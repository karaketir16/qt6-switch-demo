#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
RYUBING_DIR="${REPO_ROOT}/third_party/ryubing"
PATCH="${REPO_ROOT}/patches/ryubing-sendmmsg-udp-destination.patch"

if [ ! -x "${DOTNET_BIN}" ]; then
    echo "Set DOTNET to the .NET 10 SDK executable." >&2
    exit 1
fi

patch_applied=0
if git -C "${RYUBING_DIR}" apply --check "${PATCH}" >/dev/null 2>&1; then
    git -C "${RYUBING_DIR}" apply "${PATCH}"
    patch_applied=1
elif ! git -C "${RYUBING_DIR}" apply --reverse --check "${PATCH}" >/dev/null 2>&1; then
    echo "Ryubing source does not match either side of ${PATCH}" >&2
    exit 1
fi
trap '[ "$patch_applied" = 0 ] || git -C "${RYUBING_DIR}" apply --reverse "${PATCH}"' EXIT

"${DOTNET_BIN}" build "${RYUBING_DIR}/Ryujinx.sln" --configuration Release
test -f "${RYUBING_DIR}/src/Ryujinx/bin/Release/net10.0/Ryujinx.dll"
test -x "${RYUBING_DIR}/src/Ryujinx/bin/Release/net10.0/Ryujinx"
