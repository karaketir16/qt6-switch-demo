#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
DEMO_DIR="${1:-${REPO_ROOT}/demo/widgets-app}"

docker run --rm \
  -e CLEAN_BUILD="${CLEAN_BUILD:-0}" \
  -v "${REPO_ROOT}:${REPO_ROOT}" \
  -w "${DEMO_DIR}" \
  "${DEVKITA64_IMAGE}" \
  bash -lc '[ "$CLEAN_BUILD" != 1 ] || make clean; make -j"$(nproc)" nro'
