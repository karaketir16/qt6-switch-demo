#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
NRO_PATH="${1:-${REPO_ROOT}/demo/widgets-app/qt6-switch-widgets-probe.nro}"
RYUBING_BIN="${REPO_ROOT}/third_party/ryubing/src/Ryujinx/bin/Release/net10.0/Ryujinx"
RYUBING_SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"

test -x "${RYUBING_BIN}" || { echo "Build Ryubing first: ./scripts/build-ryubing.sh" >&2; exit 1; }
test -f "${NRO_PATH}" || { echo "NRO not found: ${NRO_PATH}" >&2; exit 1; }
mkdir -p "${RYUBING_SDCARD}"
touch "${RYUBING_SDCARD}/qt6-switch-emulator"
exec "${RYUBING_BIN}" "${NRO_PATH}"
