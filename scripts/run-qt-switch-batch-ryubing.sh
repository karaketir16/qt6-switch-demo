#!/usr/bin/env bash
set -euo pipefail

# Run every registered QtTest source in one batch NRO launch. The guest runner
# reads --all from the selector and invokes each registered qExec entry in turn.
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
NRO_PATH="${1:?usage: $0 <batch.nro> [gdb-port]}"
GDB_PORT="${2:-0}"
RYUBING_BIN="${REPO_ROOT}/third_party/ryubing/src/Ryujinx/bin/Release/net10.0/Ryujinx"
SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
SELECTOR="${SDCARD}/qt-switch-batch-case.txt"
LOG="${SDCARD}/qt-switch-batch.log"
LOCK_DIR="${SDCARD}/.qt-switch-batch-ryubing.lock"
RESULT_DIR="${REPO_ROOT}/build/test-results"
RESULT_LOG="${RESULT_DIR}/$(basename "${NRO_PATH%.nro}").batch.log"
RYUBING_LOG="${RESULT_DIR}/$(basename "${NRO_PATH%.nro}").ryubing.log"
ZONEINFO_SOURCE="${QT_SWITCH_ZONEINFO_SOURCE:-/usr/share/zoneinfo}"

case "$(basename "${NRO_PATH%.nro}")" in
    qtbase_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-manifest.txt" ;;
    qtbase_extra_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-extra-manifest.txt" ;;
    qtbase_thread_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-thread-manifest.txt" ;;
    qtbase_text_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-text-manifest.txt" ;;
    qtbase_misc_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-misc-manifest.txt" ;;
    qtbase_broad_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-broad-manifest.txt" ;;
    qtbase_value_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-value-manifest.txt" ;;
    qtbase_serialization_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtbase-serialization-manifest.txt" ;;
    qtdeclarative_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtdeclarative-manifest.txt" ;;
    qtdeclarative_extra_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtdeclarative-extra-manifest.txt" ;;
    qtdeclarative_v4_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtdeclarative-v4-manifest.txt" ;;
    qtdeclarative_meta_tests_batch) MANIFEST="${REPO_ROOT}/demo/qt-switch-batch/qtdeclarative-meta-manifest.txt" ;;
    *) echo "Unknown batch NRO; add its manifest mapping to this launcher." >&2; exit 2 ;;
esac
EXPECTED_TESTS="$(awk -F'|' 'NF && $1 !~ /^#/ { count++ } END { print count + 0 }' "${MANIFEST}")"

test -x "${RYUBING_BIN}"
test -f "${NRO_PATH}"
mkdir -p "${SDCARD}" "${RESULT_DIR}"
if ! mkdir "${LOCK_DIR}" 2>/dev/null; then
    echo "A Qt Switch batch is already using ${SDCARD}; wait for it to finish." >&2
    exit 2
fi
release_lock() {
    rmdir "${LOCK_DIR}" 2>/dev/null || true
}
trap release_lock EXIT
if [ -d "${ZONEINFO_SOURCE}" ]; then
    mkdir -p "${SDCARD}/qt6-switch-zoneinfo"
    cp -R "${ZONEINFO_SOURCE}/." "${SDCARD}/qt6-switch-zoneinfo/"
fi
mkdir -p "${SDCARD}/data"
cp "${REPO_ROOT}/third_party/qtbase/tests/auto/corelib/text/qchar/data/NormalizationTest.txt" \
    "${SDCARD}/data/NormalizationTest.txt"
if [ -n "${QT_SWITCH_BATCH_SELECTOR_FILE:-}" ]; then
    test -f "${QT_SWITCH_BATCH_SELECTOR_FILE}"
    cp "${QT_SWITCH_BATCH_SELECTOR_FILE}" "${SELECTOR}"
elif [ -n "${QT_SWITCH_BATCH_QTEST_ARGS:-}" ]; then
    # Deliberately shell-like enough for QtTest's simple flags and paths, without
    # executing caller input. Example: '-v2 -o sdmc:/qt-test-detail.txt,txt'.
    read -r -a qtest_args <<<"${QT_SWITCH_BATCH_QTEST_ARGS}"
    printf '%s\n' "--all -- ${qtest_args[*]}" > "${SELECTOR}"
else
    printf '%s\n' --all > "${SELECTOR}"
fi
: > "${LOG}"

RYUJINX_ARGS=(--no-gui)
if [ "${RYUBING_USE_HYPERVISOR:-true}" = false ]; then
    RYUJINX_ARGS+=(--use-hypervisor false)
fi
if [ "${GDB_PORT}" != 0 ]; then
    RYUJINX_ARGS+=(--enable-gdb-stub --gdb-stub-port "${GDB_PORT}" --suspend-on-start)
fi

DOTNET_ROOT="${DOTNET_ROOT}" DOTNET_ROOT_ARM64="${DOTNET_ROOT_ARM64}" \
    "${RYUBING_BIN}" "${RYUJINX_ARGS[@]}" "${NRO_PATH}" >"${RYUBING_LOG}" 2>&1 &
ryubing_pid=$!
stop_ryubing() {
    if kill -0 "${ryubing_pid}" 2>/dev/null; then
        kill "${ryubing_pid}" 2>/dev/null || true
    fi
    wait "${ryubing_pid}" 2>/dev/null || true
}
trap 'stop_ryubing; release_lock' EXIT INT TERM

while kill -0 "${ryubing_pid}" 2>/dev/null; do
    # An NRO does not make Ryubing exit after returning from appletMainLoop().
    # The batch runner writes this marker only after every selected QtTest entry
    # has returned, so it is safe to close precisely the emulator we launched.
    finished_tests="$(grep -Ec '^(PASS|FAIL) ' "${LOG}" || true)"
    if grep -q '^COMPLETE\(\|_WITH_FAILURES\) ' "${LOG}" \
        || [ "${finished_tests}" -ge "${EXPECTED_TESTS}" ]; then
        stop_ryubing
        break
    fi
    # Ryubing stays open after an NRO calls appletExit(). If QtTest died before
    # the runner could append PASS/FAIL, this is an incomplete failure, not a
    # reason to retain this launcher-owned emulator forever.
    if grep -q 'ServiceAm Exit:' "${RYUBING_LOG}" 2>/dev/null; then
        stop_ryubing
        break
    fi
    sleep 1
done
wait "${ryubing_pid}" || true
cp "${LOG}" "${RESULT_LOG}"
cat "${RESULT_LOG}"

if grep -q '^FAIL ' "${RESULT_LOG}" || ! grep -q '^PASS ' "${RESULT_LOG}"; then
    exit 1
fi
