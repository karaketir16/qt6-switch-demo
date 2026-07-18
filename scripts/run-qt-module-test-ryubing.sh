#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
exec "${REPO_ROOT}/scripts/run-qt-test-ryubing.sh" \
    "${1:-${REPO_ROOT}/demo/qt-module-test/qt6-switch-module-test.nro}" \
    qt6-switch-module-test.log module-test
