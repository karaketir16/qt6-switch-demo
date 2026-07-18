#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-host}"

mkdir -p "${BUILD_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        cmake --fresh -S '${QTBASE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=ON \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DFEATURE_dbus=OFF \
            -DFEATURE_network=OFF \
            -DINPUT_opengl=no \
            -DFEATURE_opengl=OFF \
            -DFEATURE_printsupport=OFF \
            -DFEATURE_sql=OFF \
            -DFEATURE_testlib=OFF \
            -DFEATURE_widgets=OFF \
            -DFEATURE_developer_build=ON &&
        cmake --build '${BUILD_DIR}' --parallel \"\$(nproc)\" --target Gui host_tools
    "

echo
echo "Host tools build completed."
echo "Repository root: ${REPO_ROOT}"
echo "Build directory: ${BUILD_DIR}"
