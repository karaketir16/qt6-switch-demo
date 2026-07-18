#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
BUILD_DIR="${1:-${REPO_ROOT}/build/qtdeclarative-switch}"
QT_SWITCH_PREFIX_VALUE="${QT_SWITCH_PREFIX:-${REPO_ROOT}/build/qtbase-switch}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${BUILD_DIR}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc 'cmake --build "$(pwd)" --parallel "$(nproc)" --target \
        Qml \
        QmlMeta \
        QmlModels \
        QmlWorkerScript \
        Quick \
        qmlplugin \
        modelsplugin \
        workerscriptplugin \
        qtquick2plugin \
        Qml_resources_1 \
        QmlMeta_resources_1 \
        QmlModels_resources_1 \
        QmlWorkerScript_resources_1 \
        Quick_resources_1'

"${REPO_ROOT}/scripts/generate-qt-artifacts.sh" "${QT_SWITCH_PREFIX_VALUE}" "${BUILD_DIR}"
