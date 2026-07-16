#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
"${REPO_ROOT}/scripts/build-widgets-probe.sh" "${REPO_ROOT}/demo/webengine-app"
