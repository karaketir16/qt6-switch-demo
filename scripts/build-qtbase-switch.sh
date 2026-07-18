#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
BUILD_DIR="${1:-${REPO_ROOT}/build/qtbase-switch}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${BUILD_DIR}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target Core Gui Network Widgets OpenGL OpenGLWidgets plugins/platforms/libqswitch.a plugins/tls/libqopensslbackend.a'

"${REPO_ROOT}/scripts/generate-qt-artifacts.sh" "${BUILD_DIR}"
