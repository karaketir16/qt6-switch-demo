#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QTWEBENGINE_DIR="${1:-${REPO_ROOT}/third_party/qtwebengine}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtwebengine-switch}"
TOOLCHAIN_FILE="${3:-${REPO_ROOT}/extras/toolchain-switch.cmake}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${4:-${REPO_ROOT}/build/qtbase-host}}"
QT_SWITCH_PREFIX="${QT_SWITCH_PREFIX:-${5:-${REPO_ROOT}/build/qtbase-switch}}"
QTDECLARATIVE_SWITCH_BUILD="${QTDECLARATIVE_SWITCH_BUILD:-${REPO_ROOT}/build/qtdeclarative-switch}"

if [ -z "${QT_HOST_PATH_VALUE}" ]; then
    echo "QT_HOST_PATH is required for QtWebEngine cross compilation." >&2
    exit 1
fi

mkdir -p "${BUILD_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    devkitpro/devkita64 \
    bash -lc "
        export DEBIAN_FRONTEND=noninteractive
        apt-get update
        apt-get install -y --no-install-recommends nodejs gperf bison flex python3-html5lib
        cmake -S '${QTWEBENGINE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DQT_QMAKE_TARGET_MKSPEC=devices/switch-aarch64-libnx-g++ \
            -DSWITCH=ON \
            -DQT_MKSPECS_DIR='${REPO_ROOT}/third_party/qtbase/mkspecs' \
            -DCMAKE_TOOLCHAIN_FILE='${TOOLCHAIN_FILE}' \
            -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE:STRING=BOTH \
            -DQt6_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Core' \
            -DQt6Gui_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Gui' \
            -DQt6Widgets_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Widgets' \
            -DQt6Network_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Network' \
            -DQt6OpenGL_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6OpenGL' \
            -DQt6PrintSupport_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6PrintSupport' \
            -DQt6BuildInternals_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6BuildInternals' \
            -DQt6Qml_DIR='${QTDECLARATIVE_SWITCH_BUILD}/lib/cmake/Qt6Qml' \
            -DQt6QmlModels_DIR='${QTDECLARATIVE_SWITCH_BUILD}/lib/cmake/Qt6QmlModels' \
            -DQt6Quick_DIR='${QTDECLARATIVE_SWITCH_BUILD}/lib/cmake/Qt6Quick' \
            -DQT_ADDITIONAL_PACKAGES_PREFIX_PATH='${QTDECLARATIVE_SWITCH_BUILD}' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DQT_FEATURE_qtpdf_build=OFF \
            -DQT_FEATURE_qtwebengine_quick_build=OFF \
            -DQT_FEATURE_webengine_extensions=OFF \
            -DQT_FEATURE_webengine_proprietary_codecs=OFF \
            -DQT_FEATURE_webengine_spellchecker=OFF \
            -DQT_FEATURE_webengine_webrtc=OFF \
            -DQT_FEATURE_webenginedriver=OFF \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF
    "

echo
echo "QtWebEngine Switch configuration completed."
echo "Build directory: ${BUILD_DIR}"
