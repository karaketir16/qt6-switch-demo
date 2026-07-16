#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QTDECLARATIVE_DIR="${1:-${REPO_ROOT}/third_party/qtdeclarative}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtdeclarative-host}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${3:-${REPO_ROOT}/build/qtbase-host}}"

mkdir -p "${BUILD_DIR}"
mkdir -p "${QT_HOST_PATH_VALUE}/lib/cmake/Qt6"
if [ ! -f "${QT_HOST_PATH_VALUE}/lib/cmake/Qt6/QtFileConfigure.txt.in" ]; then
    cp "${REPO_ROOT}/third_party/qtbase/cmake/QtFileConfigure.txt.in" \
        "${QT_HOST_PATH_VALUE}/lib/cmake/Qt6/QtFileConfigure.txt.in"
fi

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    devkitpro/devkita64 \
    bash -lc "
        cmake -S '${QTDECLARATIVE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DQT_MKSPECS_DIR='${REPO_ROOT}/third_party/qtbase/mkspecs' \
            -DQt6_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6Core' \
            -DQt6BuildInternals_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6BuildInternals' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DFEATURE_developer_build=ON &&
        cmake --build '${BUILD_DIR}' --parallel \"\$(nproc)\" --target host_tools
    "

echo
echo "QtDeclarative host tools build completed."
echo "Build directory: ${BUILD_DIR}"
