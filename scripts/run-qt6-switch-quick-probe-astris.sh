#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
if [ -f "${REPO_ROOT}/.env" ]; then
    set -a
    . "${REPO_ROOT}/.env"
    set +a
fi

NRO_PATH="${1:-${REPO_ROOT}/demo/quick-app/qt6-switch-quick-probe.nro}"
ASTRIS_APP="${ASTRIS_APP:-/Volumes/T7/Applications/Astris/Astris.app}"
ASTRIS_DATA="${ASTRIS_DATA:-/Volumes/T7/astrisData}"
TARGET_DIR="${ASTRIS_DATA}/homebrew/qt6-switch-quick-probe"
LOG_DIR="${HOME}/Library/Containers/V380-Ori.Astris/Data/Library/Logs/Ryujinx"
GUEST_TRACE="${ASTRIS_DATA}/sdcard/qt6-switch-quick-probe.log"
STARTUP_TRACE="${ASTRIS_DATA}/sdcard/qt6-switch-startup.log"
DEBUG_MARKER="${ASTRIS_DATA}/sdcard/qt6-switch-debug"

handle_astris_restore_prompt() {
    osascript >/dev/null 2>&1 <<'APPLESCRIPT' || true
tell application "System Events"
    repeat 20 times
        if exists process "Astris" then
            tell process "Astris"
                if exists button "Don’t Reopen" of window 1 then
                    click button "Don’t Reopen" of window 1
                    return
                end if
                if exists button "Don't Reopen" of window 1 then
                    click button "Don't Reopen" of window 1
                    return
                end if
            end tell
        end if
        delay 0.25
    end repeat
end tell
APPLESCRIPT
}

mkdir -p "${TARGET_DIR}"
cp -f "${NRO_PATH}" "${TARGET_DIR}/qt6-switch-quick-probe.nro"
rm -f "${GUEST_TRACE}"
rm -f "${STARTUP_TRACE}"
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${DEBUG_MARKER}"
else
    rm -f "${DEBUG_MARKER}"
fi

echo "Launching Astris with:"
echo "  ${TARGET_DIR}/qt6-switch-quick-probe.nro"
echo
echo "Closing any previous Astris instance before starting the QtQuick demo."
echo

pkill -x Astris || true
sleep 1

open -a "${ASTRIS_APP}" "${TARGET_DIR}/qt6-switch-quick-probe.nro"
handle_astris_restore_prompt

sleep 10

if [ -d "${LOG_DIR}" ]; then
    latest_log="$(ls -t "${LOG_DIR}"/Ryujinx_*.log 2>/dev/null | head -1 || true)"
    if [ -n "${latest_log}" ]; then
        echo "Latest Astris log: ${latest_log}"
        tail -160 "${latest_log}"
    fi
fi

if [ -f "${GUEST_TRACE}" ]; then
    echo
    echo "Guest trace:"
    cat "${GUEST_TRACE}"
fi
