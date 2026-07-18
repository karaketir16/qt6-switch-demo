#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${1:-${REPO_ROOT}/demo/qt-module-test/qt6-switch-module-test.nro}"
ASTRIS_APP="${ASTRIS_APP:-/Volumes/T7/Applications/Astris/Astris.app}"
ASTRIS_DATA="${ASTRIS_DATA:-/Volumes/T7/astrisData}"
TARGET_DIR="${ASTRIS_DATA}/homebrew/qt6-switch-module-test"
LOG_DIR="${HOME}/Library/Containers/V380-Ori.Astris/Data/Library/Logs/Ryujinx"
GUEST_TRACE="${ASTRIS_DATA}/sdcard/qt6-switch-module-test.log"

mkdir -p "${TARGET_DIR}"
cp -f "${NRO_PATH}" "${TARGET_DIR}/qt6-switch-module-test.nro"
rm -f "${GUEST_TRACE}"
pkill -x Astris || true
sleep 1
open -a "${ASTRIS_APP}" "${TARGET_DIR}/qt6-switch-module-test.nro"

sleep 8
if [ -d "${LOG_DIR}" ]; then
    latest_log="$(ls -t "${LOG_DIR}"/Ryujinx_*.log 2>/dev/null | head -1 || true)"
    if [ -n "${latest_log}" ]; then
        echo "Latest Astris log: ${latest_log}"
        tail -120 "${latest_log}"
    fi
fi
if [ -f "${GUEST_TRACE}" ]; then
    echo
    echo "Module test guest trace:"
    cat "${GUEST_TRACE}"
fi
