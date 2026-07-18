#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${1:-${REPO_ROOT}/demo/qt-network-test/qt6-switch-network-test.nro}"
ASTRIS_APP="${ASTRIS_APP:-/Volumes/T7/Applications/Astris/Astris.app}"
ASTRIS_DATA="${ASTRIS_DATA:-/Volumes/T7/astrisData}"
TARGET_DIR="${ASTRIS_DATA}/homebrew/qt6-switch-network-test"
GUEST_TRACE="${ASTRIS_DATA}/sdcard/qt6-switch-network-test.log"

mkdir -p "${TARGET_DIR}"
cp -f "${NRO_PATH}" "${TARGET_DIR}/qt6-switch-network-test.nro"
rm -f "${GUEST_TRACE}"
rm -f "${ASTRIS_DATA}/sdcard/qt6-switch-ca-bundle.pem"
touch "${ASTRIS_DATA}/sdcard/qt6-switch-emulator"
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${ASTRIS_DATA}/sdcard/qt6-switch-debug"
else
    rm -f "${ASTRIS_DATA}/sdcard/qt6-switch-debug"
fi
pkill -x Astris || true
sleep 1
open -a "${ASTRIS_APP}" "${TARGET_DIR}/qt6-switch-network-test.nro"
for _ in $(seq 1 40); do
    [ -f "${GUEST_TRACE}" ] && grep -q '^network-test: ' "${GUEST_TRACE}" && break
    sleep 1
done
if [ -f "${GUEST_TRACE}" ] && grep -q '^network-test: ' "${GUEST_TRACE}"; then
    cat "${GUEST_TRACE}"
    if grep -q '^FAIL ' "${GUEST_TRACE}"; then
        exit 1
    fi
else
    echo "Missing or incomplete guest trace: ${GUEST_TRACE}" >&2
    exit 1
fi
