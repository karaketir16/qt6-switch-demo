#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${1:-${REPO_ROOT}/demo/qt-network-test/qt6-switch-network-test.nro}"
ASTRIS_APP="${ASTRIS_APP:-/Volumes/T7/Applications/Astris/Astris.app}"
ASTRIS_DATA="${ASTRIS_DATA:-/Volumes/T7/astrisData}"
TARGET_DIR="${ASTRIS_DATA}/homebrew/qt6-switch-network-test"
GUEST_TRACE="${ASTRIS_DATA}/sdcard/qt6-switch-network-test.log"
GUEST_CA_BUNDLE="${ASTRIS_DATA}/sdcard/qt6-switch-ca-bundle.pem"

find_ca_bundle() {
    local candidate
    for candidate in \
        "${QT_SWITCH_CA_BUNDLE_SOURCE:-}" \
        /etc/ssl/cert.pem \
        /opt/homebrew/etc/ca-certificates/cert.pem \
        /opt/homebrew/share/ca-certificates/cacert.pem \
        /usr/local/etc/ca-certificates/cert.pem; do
        if [ -n "${candidate}" ] && [ -f "${candidate}" ]; then
            printf '%s\n' "${candidate}"
            return 0
        fi
    done
    return 1
}

mkdir -p "${TARGET_DIR}"
cp -f "${NRO_PATH}" "${TARGET_DIR}/qt6-switch-network-test.nro"
rm -f "${GUEST_TRACE}"
touch "${ASTRIS_DATA}/sdcard/qt6-switch-emulator"
if CA_BUNDLE="$(find_ca_bundle)"; then
    cp -f "${CA_BUNDLE}" "${GUEST_CA_BUNDLE}"
    echo "Staged CA bundle: ${CA_BUNDLE}"
else
    rm -f "${GUEST_CA_BUNDLE}"
    echo "No host CA bundle found; native Qt HTTPS is expected to reject public certificates." >&2
fi
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${ASTRIS_DATA}/sdcard/qt6-switch-debug"
else
    rm -f "${ASTRIS_DATA}/sdcard/qt6-switch-debug"
fi
pkill -x Astris || true
sleep 1
open -a "${ASTRIS_APP}" "${TARGET_DIR}/qt6-switch-network-test.nro"
for _ in $(seq 1 40); do
    [ -f "${GUEST_TRACE}" ] && break
    sleep 1
done
if [ -f "${GUEST_TRACE}" ]; then
    cat "${GUEST_TRACE}"
else
    echo "Missing guest trace: ${GUEST_TRACE}" >&2
    exit 1
fi
