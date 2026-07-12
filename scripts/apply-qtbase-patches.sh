#!/usr/bin/env bash
set -euo pipefail

QTBASE_DIR="${1:-/Volumes/T7/sdk/qt6-switch-src/qtbase}"
PATCH_DIR="${2:-$(cd "$(dirname "$0")/.." && pwd)/patches}"

if [ ! -d "${QTBASE_DIR}" ]; then
    echo "qtbase directory not found: ${QTBASE_DIR}" >&2
    exit 1
fi

if [ ! -d "${PATCH_DIR}" ]; then
    echo "patch directory not found: ${PATCH_DIR}" >&2
    exit 1
fi

git -C "${QTBASE_DIR}" am "${PATCH_DIR}"/000*.patch

