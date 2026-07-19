#!/usr/bin/env bash
set -euo pipefail

# Build the two single-executable QtTest batches. QtBase/QtDeclarative and
# their host tools are prerequisites; this script does not build them.
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
BATCH_BUILD_DIR="${BATCH_BUILD_DIR:-${REPO_ROOT}/build/qt-switch-batch}"
QTBASE_BUILD_DIR="${QTBASE_BUILD_DIR:-${REPO_ROOT}/build/qtbase-switch-clean}"
QTBASE_PREFIX="${QTBASE_PREFIX:-${REPO_ROOT}/build/qtbase-switch}"
QTDECLARATIVE_PREFIX="${QTDECLARATIVE_PREFIX:-${REPO_ROOT}/build/qtdeclarative-switch}"
QT_CMAKE_MODULE_PATH="${QT_CMAKE_MODULE_PATH:-${REPO_ROOT}/build/qtbase-switch/lib/cmake/Qt6}"
QML_TOOLS_OVERLAY="${QML_TOOLS_OVERLAY:-${REPO_ROOT}/build/qtdeclarative-batch-qmltools-overlay}"
BUILD_QTDECLARATIVE="${BUILD_QTDECLARATIVE:-OFF}"

if [ "${BUILD_QTDECLARATIVE}" = ON ]; then
    mkdir -p "${QML_TOOLS_OVERLAY}/lib/cmake/Qt6QmlTools"
    cat > "${QML_TOOLS_OVERLAY}/lib/cmake/Qt6QmlTools/Qt6QmlToolsConfig.cmake" <<EOF
set(Qt6QmlTools_FOUND TRUE)
set(Qt6QmlTools_LIBRARIES "")
EOF
    cat > "${QML_TOOLS_OVERLAY}/lib/cmake/Qt6QmlTools/Qt6QmlToolsConfigVersion.cmake" <<EOF
set(PACKAGE_VERSION "6.8.3")
set(PACKAGE_VERSION_COMPATIBLE TRUE)
set(PACKAGE_VERSION_EXACT TRUE)
EOF
fi

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        if [ '${BUILD_QTDECLARATIVE}' = ON ]; then
            cmake --build '${QTDECLARATIVE_PREFIX}' --parallel \$(nproc) --target \\
                qmlplugin modelsplugin workerscriptplugin \\
                qmlplugin_init modelsplugin_init workerscriptplugin_init \\
                QmlMeta_resources_1 QmlModels_resources_1 QmlWorkerScript_resources_1
        fi
        cmake --fresh -S '${REPO_ROOT}/demo/qt-switch-batch' \
            -B '${BATCH_BUILD_DIR}' -GNinja \
            -DCMAKE_TOOLCHAIN_FILE='${REPO_ROOT}/extras/toolchain-switch.cmake' \
            -DCMAKE_MODULE_PATH='${QT_CMAKE_MODULE_PATH};${QTBASE_PREFIX}/lib/cmake/Qt6' \
            -DCMAKE_PREFIX_PATH='${QTDECLARATIVE_PREFIX};${QTBASE_BUILD_DIR}' \
            -DQT_SWITCH_BUILD_QTDECLARATIVE='${BUILD_QTDECLARATIVE}' \
            -DQt6_DIR='${QTBASE_BUILD_DIR}/lib/cmake/Qt6' \
            -DQt6Core_DIR='${QTBASE_BUILD_DIR}/lib/cmake/Qt6Core' \
            -DQt6Gui_DIR='${QTBASE_BUILD_DIR}/lib/cmake/Qt6Gui' \
            -DQt6Test_DIR='${QTBASE_BUILD_DIR}/lib/cmake/Qt6Test' \
            -DQt6Qml_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6Qml' \
            -DQt6QmlIntegration_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6QmlIntegration' \
            -DQt6QmlCore_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6QmlCore' \
            -DQt6QmlModels_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6QmlModels' \
            -DQt6QmlMeta_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6QmlMeta' \
            -DQt6QmlWorkerScript_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6QmlWorkerScript' \
            -DQt6modelsplugin_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6Qml/QmlPlugins' \
            -DQt6workerscriptplugin_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6Qml/QmlPlugins' \
            -DQt6qmlplugin_DIR='${QTDECLARATIVE_PREFIX}/lib/cmake/Qt6Qml/QmlPlugins' \
            -DQt6QmlTools_DIR='${QML_TOOLS_OVERLAY}/lib/cmake/Qt6QmlTools'
        cmake --build '${BATCH_BUILD_DIR}' --parallel \$(nproc)
    "

echo "Batch build directory: ${BATCH_BUILD_DIR}"
