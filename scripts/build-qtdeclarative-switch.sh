#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="${1:-${REPO_ROOT}/build/qtdeclarative-switch}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${BUILD_DIR}" \
    devkitpro/devkita64 \
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
