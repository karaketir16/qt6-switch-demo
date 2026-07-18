#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${1:-${REPO_ROOT}/demo/qt-network-test/qt6-switch-network-test.nro}"
RYUBING_APP="${RYUBING_APP:-/Volumes/T7/Ryubing/Ryujinx.app/Contents/MacOS/Ryujinx}"
RYUBING_SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
GUEST_TRACE="${RYUBING_SDCARD}/qt6-switch-network-test.log"
GUEST_LOGS=(
    "${GUEST_TRACE}"
    "${RYUBING_SDCARD}/qt6-switch-probe.log"
    "${RYUBING_SDCARD}/qt6-switch-startup.log"
)

mkdir -p "${RYUBING_SDCARD}"
rm -f "${GUEST_LOGS[@]}"
rm -f "${RYUBING_SDCARD}/qt6-switch-ca-bundle.pem"
: > /tmp/ryubing-network.stdout
: > /tmp/ryubing-network.stderr
touch "${RYUBING_SDCARD}/qt6-switch-emulator"
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${RYUBING_SDCARD}/qt6-switch-debug"
else
    rm -f "${RYUBING_SDCARD}/qt6-switch-debug"
fi

# The same NRO can have been launched from the packaged app or a source build.
# Stop only prior instances of this test before starting a new one.
pkill -f "${NRO_PATH}" || true
pkill -f "${RYUBING_APP}" || true
"${RYUBING_APP}" "${NRO_PATH}" >/tmp/ryubing-network.stdout 2>/tmp/ryubing-network.stderr &
for _ in $(seq 1 40); do
    [ -f "${GUEST_TRACE}" ] && grep -q '^network-test: ' "${GUEST_TRACE}" && break
    sleep 1
done
if [ -f "${GUEST_TRACE}" ] && grep -q '^network-test: ' "${GUEST_TRACE}"; then
    cat "${GUEST_TRACE}"
    grep -q '^FAIL ' "${GUEST_TRACE}" && exit 1
else
    echo "Missing or incomplete guest trace: ${GUEST_TRACE}" >&2
    exit 1
fi
