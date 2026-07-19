#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
RYUBING_DIR="${REPO_ROOT}/third_party/ryubing"
PATCHES=(
    "${REPO_ROOT}/patches/ryubing-sendmmsg-udp-destination.patch"
    "${REPO_ROOT}/patches/ryubing-thread-limit-diagnostics.patch"
)

if [ ! -x "${DOTNET_BIN}" ]; then
    echo "Set DOTNET to the .NET 10 SDK executable." >&2
    exit 1
fi

applied_patches=()
for patch_file in "${PATCHES[@]}"; do
    if git -C "${RYUBING_DIR}" apply --check "${patch_file}" >/dev/null 2>&1; then
        git -C "${RYUBING_DIR}" apply "${patch_file}"
        applied_patches+=("${patch_file}")
    elif ! git -C "${RYUBING_DIR}" apply --reverse --check "${patch_file}" >/dev/null 2>&1; then
        echo "Ryubing source does not match either side of ${patch_file}" >&2
        exit 1
    fi
done
restore_patches() {
    local patch_file
    for patch_file in "${applied_patches[@]}"; do
        git -C "${RYUBING_DIR}" apply --reverse "${patch_file}"
    done
}
trap restore_patches EXIT

"${DOTNET_BIN}" build "${RYUBING_DIR}/Ryujinx.sln" --configuration Release
test -f "${RYUBING_DIR}/src/Ryujinx/bin/Release/net10.0/Ryujinx.dll"
test -x "${RYUBING_DIR}/src/Ryujinx/bin/Release/net10.0/Ryujinx"
