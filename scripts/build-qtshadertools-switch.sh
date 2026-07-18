#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
BUILD_DIR="${1:-${REPO_ROOT}/build/qtshadertools-switch}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${BUILD_DIR}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target ShaderTools'
