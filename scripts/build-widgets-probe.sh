#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DEMO_DIR="${1:-${REPO_ROOT}/demo/widgets-app}"

docker run --rm \
  -v "${REPO_ROOT}:${REPO_ROOT}" \
  -w "${DEMO_DIR}" \
  devkitpro/devkita64 \
  make clean nro
