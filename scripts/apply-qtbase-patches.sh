#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
PATCH_DIR="${2:-${REPO_ROOT}/patches}"

if [ ! -d "${QTBASE_DIR}" ]; then
    echo "qtbase directory not found: ${QTBASE_DIR}" >&2
    exit 1
fi

if [ ! -d "${PATCH_DIR}" ]; then
    echo "patch directory not found: ${PATCH_DIR}" >&2
    exit 1
fi

git -C "${QTBASE_DIR}" am "${PATCH_DIR}"/000*.patch
