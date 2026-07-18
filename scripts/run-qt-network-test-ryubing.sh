#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${1:-${REPO_ROOT}/demo/qt-network-test/qt6-switch-network-test.nro}"
RYUBING_APP="${RYUBING_APP:-/Volumes/T7/Ryubing/Ryujinx.app/Contents/MacOS/Ryujinx}"
RYUBING_SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
GUEST_TRACE="${RYUBING_SDCARD}/qt6-switch-network-test.log"
GUEST_CA_BUNDLE="${RYUBING_SDCARD}/qt6-switch-ca-bundle.pem"

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

mkdir -p "${RYUBING_SDCARD}"
rm -f "${GUEST_TRACE}"
touch "${RYUBING_SDCARD}/qt6-switch-emulator"
if CA_BUNDLE="$(find_ca_bundle)"; then
    cp -f "${CA_BUNDLE}" "${GUEST_CA_BUNDLE}"
    echo "Staged CA bundle: ${CA_BUNDLE}"
else
    rm -f "${GUEST_CA_BUNDLE}"
    echo "No host CA bundle found; native Qt HTTPS is expected to reject public certificates." >&2
fi
if [ "${QT_SWITCH_DEBUG_LOG:-0}" = 1 ]; then
    touch "${RYUBING_SDCARD}/qt6-switch-debug"
else
    rm -f "${RYUBING_SDCARD}/qt6-switch-debug"
fi

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
