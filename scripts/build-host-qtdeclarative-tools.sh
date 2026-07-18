#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTDECLARATIVE_DIR="${1:-${REPO_ROOT}/third_party/qtdeclarative}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtdeclarative-host}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${3:-${REPO_ROOT}/build/qtbase-host}}"
QTSHADERTOOLS_HOST_BUILD="${QTSHADERTOOLS_HOST_BUILD:-${REPO_ROOT}/build/qtshadertools-host}"
QT_CMAKE_OVERLAY_DIR="${QT_CMAKE_OVERLAY_DIR:-${REPO_ROOT}/build/qtbase-host-cmake-overlay}"
QT_BASE_CMAKE_DIR="${QT_HOST_PATH_VALUE}/lib/cmake/Qt6"
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

mkdir -p "${BUILD_DIR}"
mkdir -p "${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6"
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
    set_target_properties(Qt::qsb PROPERTIES IMPORTED_LOCATION "${QSB_PATH}")
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
        cmake --fresh -S '${QTDECLARATIVE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DCMAKE_BUILD_TYPE=Release \
            -DQT_MKSPECS_DIR='${REPO_ROOT}/third_party/qtbase/mkspecs' \
            -DQt6_DIR='${QT_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6Core' \
            -DQt6ShaderTools_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6ShaderTools' \
            -DQt6ShaderToolsTools_DIR='${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR}/lib/cmake/Qt6ShaderToolsTools' \
            -DQT_SWITCH_QTSHADERTOOLS_MACROS='${REPO_ROOT}/third_party/qtshadertools/tools/qsb/Qt6ShaderToolsMacros.cmake' \
            -DQt6BuildInternals_DIR='${QT_HOST_PATH_VALUE}/lib/cmake/Qt6BuildInternals' \
            -DQT_ADDITIONAL_HOST_PACKAGES_PREFIX_PATH='${QTSHADERTOOLS_HOST_CMAKE_OVERLAY_DIR};${QT_HOST_PATH_VALUE}' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DFEATURE_developer_build=ON &&
        cmake --build '${BUILD_DIR}' --parallel \"\$(nproc)\" --target host_tools
    "

echo
echo "QtDeclarative host tools build completed."
echo "Build directory: ${BUILD_DIR}"
