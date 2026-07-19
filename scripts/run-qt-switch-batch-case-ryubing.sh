#!/usr/bin/env bash
set -euo pipefail

# Run one case from a single batch NRO. No wall-clock timeout is used.
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
NRO_PATH="${1:?usage: $0 <batch.nro> <case-name> [gdb-port]}"
CASE_NAME="${2:?usage: $0 <batch.nro> <case-name> [gdb-port]}"
GDB_PORT="${3:-0}"
RYUBING_BIN="${REPO_ROOT}/third_party/ryubing/src/Ryujinx/bin/Release/net10.0/Ryujinx"
SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
SELECTOR="${SDCARD}/qt-switch-batch-case.txt"
LOG="${SDCARD}/qt-switch-batch.log"
LOCK_DIR="${SDCARD}/.qt-switch-batch-ryubing.lock"
RESULT_DIR="${REPO_ROOT}/build/test-results"
RYUBING_LOG="${RESULT_DIR}/qt-switch-batch-${CASE_NAME}.ryubing.log"

test -x "${RYUBING_BIN}"
test -f "${NRO_PATH}"
mkdir -p "${SDCARD}"
mkdir -p "${RESULT_DIR}"
if ! mkdir "${LOCK_DIR}" 2>/dev/null; then
    echo "A Qt Switch batch is already using ${SDCARD}; wait for it to finish." >&2
    exit 2
fi
release_lock() {
    rmdir "${LOCK_DIR}" 2>/dev/null || true
}
trap release_lock EXIT
SELECTOR_ARGS=("${CASE_NAME}")
if [ "$#" -gt 3 ]; then
    SELECTOR_ARGS+=("${@:4}")
fi
printf '%s ' "${SELECTOR_ARGS[@]}" > "${SELECTOR}"
printf '\n' >> "${SELECTOR}"
: > "${LOG}"

RYUJINX_ARGS=(--no-gui)
if [ "${GDB_PORT}" != 0 ]; then
    RYUJINX_ARGS+=(--enable-gdb-stub --gdb-stub-port "${GDB_PORT}" --suspend-on-start)
fi

DOTNET_ROOT="${DOTNET_ROOT}" DOTNET_ROOT_ARM64="${DOTNET_ROOT_ARM64}" \
    "${RYUBING_BIN}" "${RYUJINX_ARGS[@]}" "${NRO_PATH}" >"${RYUBING_LOG}" 2>&1 &
ryubing_pid=$!
stop_ryubing() {
    kill "${ryubing_pid}" 2>/dev/null || true
    wait "${ryubing_pid}" 2>/dev/null || true
}
trap 'stop_ryubing; release_lock' EXIT INT TERM

while :; do
    if grep -qE "^(PASS|FAIL) ${CASE_NAME}$" "${LOG}" 2>/dev/null; then
        grep -E "^(BEGIN|PASS|FAIL) ${CASE_NAME}$" "${LOG}"
        grep -q "^PASS ${CASE_NAME}$" "${LOG}"
        exit $?
    fi
    if ! kill -0 "${ryubing_pid}" 2>/dev/null; then
        echo "Ryubing exited before ${CASE_NAME} completed" >&2
        tail -80 "${RYUBING_LOG}" >&2
        exit 1
    fi
    if grep -q 'ServiceAm Exit:' "${RYUBING_LOG}" 2>/dev/null; then
        echo "${CASE_NAME} exited before its batch result was written" >&2
        tail -80 "${RYUBING_LOG}" >&2
        exit 1
    fi
    sleep 1
done
