#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
docker run --rm \
    -e CLEAN_BUILD="${CLEAN_BUILD:-0}" \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}/demo/qt-network-test" \
    "${DEVKITA64_IMAGE}" \
    bash -lc '[ "$CLEAN_BUILD" != 1 ] || make clean; make -j"$(nproc)"'
