#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTSHADERTOOLS_DIR="${1:-${REPO_ROOT}/third_party/qtshadertools}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtshadertools-host}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${3:-${REPO_ROOT}/build/qtbase-host}}"
QT_CMAKE_OVERLAY_DIR="${QT_CMAKE_OVERLAY_DIR:-${REPO_ROOT}/build/qtbase-host-cmake-overlay}"
QT_BASE_CMAKE_DIR="${QT_HOST_PATH_VALUE}/lib/cmake/Qt6"

mkdir -p "${BUILD_DIR}" "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6"
cp -R "${QT_BASE_CMAKE_DIR}/." "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6/"
cp "${REPO_ROOT}/third_party/qtbase/cmake/QtFileConfigure.txt.in" \
    "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6/QtFileConfigure.txt.in"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        cmake --fresh -S '${QTSHADERTOOLS_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DQT_MKSPECS_DIR='${REPO_ROOT}/third_party/qtbase/mkspecs' \
            -DQt6_DIR='${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6Core' \
            -DQt6Gui_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6Gui' \
            -DQt6BuildInternals_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6BuildInternals' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DFEATURE_developer_build=ON &&
        cmake --build '${BUILD_DIR}' --parallel \"\$(nproc)\" --target host_tools
    "

echo
echo "QtShaderTools host tools build completed."
echo "Build directory: ${BUILD_DIR}"
