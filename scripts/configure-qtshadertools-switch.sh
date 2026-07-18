#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTSHADERTOOLS_DIR="${1:-${REPO_ROOT}/third_party/qtshadertools}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtshadertools-switch}"
TOOLCHAIN_FILE="${3:-${REPO_ROOT}/extras/toolchain-switch.cmake}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${4:-${REPO_ROOT}/build/qtbase-host}}"
QT_SWITCH_PREFIX="${QT_SWITCH_PREFIX:-${5:-${REPO_ROOT}/build/qtbase-switch}}"
QTSHADERTOOLS_HOST_BUILD="${QTSHADERTOOLS_HOST_BUILD:-${REPO_ROOT}/build/qtshadertools-host}"
QT_CMAKE_OVERLAY_DIR="${QT_CMAKE_OVERLAY_DIR:-${REPO_ROOT}/build/qtbase-switch-cmake-overlay}"
QT_BASE_CMAKE_DIR="${QT_SWITCH_PREFIX}/lib/cmake/Qt6"
QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR="${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR:-${REPO_ROOT}/build/qtshadertools-host-cmake-overlay}"

QSB_PATH=""
for candidate in \
    "${QTSHADERTOOLS_HOST_BUILD}/bin/qsb" \
    "${QTSHADERTOOLS_HOST_BUILD}/libexec/qsb" \
    "${QT_HOST_PATH_VALUE}/bin/qsb"; do
    if [ -f "${candidate}" ] && [ -x "${candidate}" ]; then
        QSB_PATH="${candidate}"
        break
    fi
done
if [ -z "${QSB_PATH}" ]; then
    echo "Missing QtShaderTools host tool: qsb" >&2
    exit 1
fi

mkdir -p "${BUILD_DIR}" "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6"
cp -R "${QT_BASE_CMAKE_DIR}/." "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6/"
cp "${REPO_ROOT}/third_party/qtbase/cmake/QtFileConfigure.txt.in" \
    "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6/QtFileConfigure.txt.in"
mkdir -p "${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6ShaderToolsTools"
cat > "${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6ShaderToolsTools/Qt6ShaderToolsToolsConfig.cmake" <<EOF
set(Qt6ShaderToolsTools_FOUND TRUE)
if(NOT TARGET Qt6::qsb)
    add_executable(Qt6::qsb IMPORTED GLOBAL)
    set_target_properties(Qt6::qsb PROPERTIES IMPORTED_LOCATION "${QSB_PATH}")
endif()
if(NOT TARGET Qt::qsb AND TARGET Qt6::qsb)
    add_executable(Qt::qsb IMPORTED GLOBAL)
    get_target_property(_qt_qsb_location Qt6::qsb IMPORTED_LOCATION)
    set_target_properties(Qt::qsb PROPERTIES IMPORTED_LOCATION "\${_qt_qsb_location}")
endif()
EOF
cat > "${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6ShaderToolsTools/Qt6ShaderToolsToolsConfigVersion.cmake" <<EOF
set(PACKAGE_VERSION "6.8.3")
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_EXACT TRUE)
EOF

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        cmake --fresh -S '${QTSHADERTOOLS_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DQT_QMAKE_TARGET_MKSPEC=devices/switch-aarch64-libnx-g++ \
            -DSWITCH=ON \
            -DQT_MKSPECS_DIR='${REPO_ROOT}/third_party/qtbase/mkspecs' \
            -DCMAKE_TOOLCHAIN_FILE='${TOOLCHAIN_FILE}' \
            -DCMAKE_BUILD_TYPE=Release \
            -DQt6_DIR='${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Core' \
            -DQt6Gui_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6Gui' \
            -DQt6BuildInternals_DIR='${QT_SWITCH_PREFIX}/lib/cmake/Qt6BuildInternals' \
            -DQt6ShaderToolsTools_DIR='${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6ShaderToolsTools' \
            -DQT_ADDITIONAL_HOST_PACKAGES_PREFIX_PATH='${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR};${QTSHADERTOOLS_HOST_BUILD}' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DQT_SKIP_AUTO_PLUGIN_INCLUSION=ON \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF
    "

echo
echo "QtShaderTools Switch configuration completed."
echo "Build directory: ${BUILD_DIR}"
