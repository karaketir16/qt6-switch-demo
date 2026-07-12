#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-host}"
INSTALL_DIR="${3:-${REPO_ROOT}/build/qtbase-host-install}"

mkdir -p "${BUILD_DIR}" "${INSTALL_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    devkitpro/devkita64 \
    bash -lc "
        cmake -S '${QTBASE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DCMAKE_INSTALL_PREFIX='${INSTALL_DIR}' \
            -DBUILD_SHARED_LIBS=ON \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DQT_FEATURE_dbus=OFF \
            -DQT_FEATURE_gui=OFF \
            -DQT_FEATURE_network=OFF \
            -DQT_FEATURE_opengl=OFF \
            -DQT_FEATURE_printsupport=OFF \
            -DQT_FEATURE_sql=OFF \
            -DQT_FEATURE_testlib=OFF \
            -DQT_FEATURE_widgets=OFF \
            -DFEATURE_developer_build=ON &&
        cmake --build '${BUILD_DIR}' --parallel \"\$(nproc)\" --target host_tools
    "

echo
echo "Host tools build completed."
echo "Repository root: ${REPO_ROOT}"
echo "Build directory: ${BUILD_DIR}"
echo "Install directory: ${INSTALL_DIR}"
