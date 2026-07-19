#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"

# The tests require QtTest and a different CMake target graph.  Keep that graph
# isolated while sharing the ordinary Switch configurator, sources, toolchain,
# host tools and OpenSSL installation.
QT_BUILD_TESTS="${QT_BUILD_TESTS:-ON}" \
QT_BUILD_TESTS_BATCHED="${QT_BUILD_TESTS_BATCHED:-ON}" \
QT_FEATURE_TESTLIB=ON \
"${REPO_ROOT}/scripts/configure-qtbase-switch.sh" \
    "${1:-${REPO_ROOT}/third_party/qtbase}" \
    "${2:-${REPO_ROOT}/build/qtbase-switch-tests}" \
    "${3:-${REPO_ROOT}/extras/toolchain-switch.cmake}" \
    "${4:-${REPO_ROOT}/build/qtbase-host}" \
    "${5:-ON}"
