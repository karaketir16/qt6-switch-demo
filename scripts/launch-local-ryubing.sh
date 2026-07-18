#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

if [ -f "${REPO_ROOT}/.env" ]; then
    set -a
    # shellcheck disable=SC1091
    . "${REPO_ROOT}/.env"
    set +a
fi

NRO_PATH="${1:-${RYUBING_NRO:-${REPO_ROOT}/demo/qt-network-test/qt6-switch-network-test.nro}}"
RYUBING_APP_PATH="${2:-${RYUBING_APP:-}}"
RYUBING_SDCARD_PATH="${3:-${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}}"

if [ -z "${RYUBING_APP_PATH}" ] || [ ! -x "${RYUBING_APP_PATH}" ]; then
    echo "Set RYUBING_APP in .env or pass the executable as argument 2." >&2
    exit 1
fi

if [ ! -f "${NRO_PATH}" ]; then
    echo "NRO not found: ${NRO_PATH}" >&2
    exit 1
fi

mkdir -p "${RYUBING_SDCARD_PATH}"
touch "${RYUBING_SDCARD_PATH}/qt6-switch-emulator"
pkill -f "${NRO_PATH}" || true

cd "$(dirname "${RYUBING_APP_PATH}")"
DOTNET_ROOT="${RYUBING_DOTNET_ROOT:-}" \
DOTNET_ROOT_ARM64="${RYUBING_DOTNET_ROOT:-}" \
exec "${RYUBING_APP_PATH}" "${NRO_PATH}"
