#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-switch}"
TOOLCHAIN_FILE="${3:-${REPO_ROOT}/extras/toolchain-switch.cmake}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${4:-${REPO_ROOT}/build/qtbase-host}}"
QT_FEATURE_WIDGETS_VALUE="${QT_FEATURE_WIDGETS:-${5:-ON}}"

if [ -z "${QT_HOST_PATH_VALUE}" ]; then
    echo "QT_HOST_PATH is required for Qt cross compilation." >&2
    echo "Example: QT_HOST_PATH=${REPO_ROOT}/build/qtbase-host ./configure-qtbase-switch.sh" >&2
    exit 1
fi

mkdir -p "${BUILD_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    devkitpro/devkita64 \
    bash -lc "
        cmake -S '${QTBASE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DQT_QMAKE_TARGET_MKSPEC=devices/switch-aarch64-libnx-g++ \
            -DCMAKE_TOOLCHAIN_FILE='${TOOLCHAIN_FILE}' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DBUILD_SHARED_LIBS=OFF \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DQT_FEATURE_testlib=OFF \
            -DQT_FEATURE_dbus=OFF \
            -DQT_FEATURE_gui=ON \
            -DQT_FEATURE_widgets='${QT_FEATURE_WIDGETS_VALUE}'
    "

echo
echo "Qt Switch configuration completed."
echo "Repository root: ${REPO_ROOT}"
echo "Build directory: ${BUILD_DIR}"
