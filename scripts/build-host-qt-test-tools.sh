#!/usr/bin/env bash
set -euo pipefail

# Build native QtTest support needed by QtDeclarative's qmltestrunner.
# This is separate from build-host-qt-tools.sh because the normal host Qt
# toolchain intentionally omits TestLib.
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-test-host}"

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
            -DFEATURE_dbus=OFF -DFEATURE_network=OFF \
            -DINPUT_opengl=no -DFEATURE_gui=ON -DFEATURE_opengl=OFF \
            -DFEATURE_printsupport=OFF \
            -DFEATURE_sql=OFF -DFEATURE_testlib=ON \
            -DFEATURE_batch_test_support=ON -DFEATURE_widgets=OFF \
            -DFEATURE_developer_build=ON
        cmake --build '${BUILD_DIR}' --parallel \$(nproc) --target Gui lib/libQt6Test.so
    "

echo "QtTest host build directory: ${BUILD_DIR}"
