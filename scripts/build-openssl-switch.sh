#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
OPENSSL_VERSION="${OPENSSL_VERSION:-3.0.16}"
BUILD_ROOT="${OPENSSL_BUILD_ROOT:-${REPO_ROOT}/build/openssl-switch}"
SOURCE_DIR="${BUILD_ROOT}/src/openssl-${OPENSSL_VERSION}"
ARCHIVE="${BUILD_ROOT}/openssl-${OPENSSL_VERSION}.tar.gz"

mkdir -p "${BUILD_ROOT}/src"
if [ ! -d "${SOURCE_DIR}" ]; then
    curl -L --fail --silent --show-error \
        "https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz" \
        -o "${ARCHIVE}"
    tar -xzf "${ARCHIVE}" -C "${BUILD_ROOT}/src"
fi

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -v "${BUILD_ROOT}:${BUILD_ROOT}" \
    -w "${REPO_ROOT}" \
    devkitpro/devkita64 \
    bash -lc "
        set -e
        export PATH=/opt/devkitpro/devkitA64/bin:\$PATH
        cd '${SOURCE_DIR}'
        if ! grep -q 'libnx exposes the console CSPRNG' providers/implementations/rands/seed_src.c; then
            patch --batch -p1 < '${REPO_ROOT}/patches/openssl-3.0.16-switch-rng.patch'
        fi
        make clean >/dev/null 2>&1 || true
        CFLAGS='-DNO_SYSLOG -DNO_SYS_PARAM_H -D__SWITCH__ -I/opt/devkitpro/libnx/include' ./Configure \
            linux-generic64 no-shared no-tests no-engine no-dso \
            no-async no-sock no-ui-console --with-rand-seed=none \
            --cross-compile-prefix=aarch64-none-elf- \
            --prefix='${BUILD_ROOT}/install'
        make build_generated
        make -j\$(nproc) libcrypto.a libssl.a
    "

rm -rf "${BUILD_ROOT}/install/include" "${BUILD_ROOT}/install/lib"
mkdir -p "${BUILD_ROOT}/install/include" "${BUILD_ROOT}/install/lib"
cp -R "${SOURCE_DIR}/include/openssl" "${BUILD_ROOT}/install/include/"
cp "${SOURCE_DIR}/libcrypto.a" "${SOURCE_DIR}/libssl.a" "${BUILD_ROOT}/install/lib/"

echo "OpenSSL Switch prefix: ${BUILD_ROOT}/install"
