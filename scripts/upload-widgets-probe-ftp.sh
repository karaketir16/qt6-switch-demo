#!/usr/bin/env bash
set -euo pipefail

HOST="${1:-192.168.1.6}"
PORT="${2:-5000}"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
NRO_PATH="${3:-${REPO_ROOT}/demo/widgets-app/qt6-switch-widgets-probe.nro}"

if [ ! -f "${NRO_PATH}" ]; then
    echo "NRO not found: ${NRO_PATH}" >&2
    exit 1
fi

curl --quote 'DELE qt6-switch-font.ttf' "ftp://${HOST}:${PORT}/" || true
curl -T "${NRO_PATH}" "ftp://${HOST}:${PORT}/switch/qt6-switch-widgets-probe.nro"
