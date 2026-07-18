#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}/demo/qt-network-test" \
    devkitpro/devkita64 \
    bash -lc 'make clean && make -j"$(nproc)"'
