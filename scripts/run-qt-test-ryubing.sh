#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
NRO_PATH="$1"
TRACE_NAME="$2"
SUMMARY_PREFIX="$3"
RYUBING_BIN="${REPO_ROOT}/third_party/ryubing/src/Ryujinx/bin/Release/net10.0/Ryujinx"
RYUBING_SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
GUEST_TRACE="${RYUBING_SDCARD}/${TRACE_NAME}"
RESULT_DIR="${REPO_ROOT}/build/test-results"
TEST_NAME="${TRACE_NAME%.log}"
STDOUT_LOG="${RESULT_DIR}/${TEST_NAME}.stdout"
STDERR_LOG="${RESULT_DIR}/${TEST_NAME}.stderr"

test -x "${RYUBING_BIN}" || { echo "Build Ryubing first: ./scripts/build-ryubing.sh" >&2; exit 1; }
test -f "${NRO_PATH}" || { echo "NRO not found: ${NRO_PATH}" >&2; exit 1; }
mkdir -p "${RYUBING_SDCARD}" "${RESULT_DIR}"
rm -f "${GUEST_TRACE}" \
    "${RYUBING_SDCARD}/qt6-switch-probe.log" \
    "${RYUBING_SDCARD}/qt6-switch-openssl-seed.log" \
    "${RYUBING_SDCARD}/qt6-switch-startup.log" \
    "${RYUBING_SDCARD}/qt6-switch-ca-bundle.pem"
: > "${STDOUT_LOG}"
: > "${STDERR_LOG}"
touch "${RYUBING_SDCARD}/qt6-switch-emulator"
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${RYUBING_SDCARD}/qt6-switch-debug"
else
    rm -f "${RYUBING_SDCARD}/qt6-switch-debug"
fi

"${RYUBING_BIN}" "${NRO_PATH}" >"${STDOUT_LOG}" 2>"${STDERR_LOG}" &
ryubing_pid=$!
stop_ryubing() {
    kill "${ryubing_pid}" 2>/dev/null || true
    wait "${ryubing_pid}" 2>/dev/null || true
}
trap stop_ryubing EXIT

for _ in $(seq 1 "${RYUBING_TIMEOUT:-30}"); do
    if [ -f "${GUEST_TRACE}" ] && grep -q "^${SUMMARY_PREFIX}: " "${GUEST_TRACE}"; then
        break
    fi
    if ! kill -0 "${ryubing_pid}" 2>/dev/null; then
        echo "Ryubing exited before producing ${GUEST_TRACE}" >&2
        tail -80 "${STDERR_LOG}" >&2
        exit 1
    fi
    if grep -Eq 'Unhandled exception|Couldn.t find any application' "${STDOUT_LOG}"; then
        echo "Ryubing failed while launching ${NRO_PATH}" >&2
        tail -80 "${STDOUT_LOG}" >&2
        exit 1
    fi
    sleep 1
done

if [ ! -f "${GUEST_TRACE}" ] || ! grep -q "^${SUMMARY_PREFIX}: " "${GUEST_TRACE}"; then
    echo "Missing or incomplete guest trace: ${GUEST_TRACE}" >&2
    tail -80 "${STDERR_LOG}" >&2
    exit 1
fi

cat "${GUEST_TRACE}"
shasum -a 256 "${NRO_PATH}"
! grep -q '^FAIL ' "${GUEST_TRACE}"
