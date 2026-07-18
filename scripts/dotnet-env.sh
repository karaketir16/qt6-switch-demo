#!/usr/bin/env bash

DOTNET_BIN="${DOTNET:-$(command -v dotnet || true)}"
if [ ! -x "${DOTNET_BIN}" ] && [ -x "${REPO_ROOT}/../tools/dotnet/dotnet" ]; then
    DOTNET_BIN="${REPO_ROOT}/../tools/dotnet/dotnet"
fi
if [ -x "${DOTNET_BIN}" ]; then
    DOTNET_ROOT="${DOTNET_ROOT:-$(cd "$(dirname "${DOTNET_BIN}")" && pwd)}"
    DOTNET_ROOT_ARM64="${DOTNET_ROOT_ARM64:-${DOTNET_ROOT}}"
    export DOTNET_ROOT DOTNET_ROOT_ARM64
fi
