#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"

QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-switch-tests}"

"${REPO_ROOT}/scripts/configure-qtbase-switch-tests.sh" "${QTBASE_DIR}" "${BUILD_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${BUILD_DIR}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc 'cmake --build . --parallel "$(nproc)" --target qtbase_tests'
