#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
OPENSSL_VERSION=3.0.16
OPENSSL_SHA256=57e03c50feab5d31b152af2b764f10379aecd8ee92f16c985983ce4a99f7ef86
BUILD_ROOT="${OPENSSL_BUILD_ROOT:-${REPO_ROOT}/build/openssl-switch}"
SOURCE_DIR="${BUILD_ROOT}/src/openssl-${OPENSSL_VERSION}"
ARCHIVE="${BUILD_ROOT}/openssl-${OPENSSL_VERSION}.tar.gz"

mkdir -p "${BUILD_ROOT}/src"
if [ ! -f "${ARCHIVE}" ]; then
    curl -L --fail --silent --show-error \
        "https://www.openssl.org/source/openssl-${OPENSSL_VERSION}.tar.gz" \
        -o "${ARCHIVE}"
fi
ACTUAL_SHA256="$(openssl dgst -sha256 -r "${ARCHIVE}" | awk '{print $1}')"
if [ "${ACTUAL_SHA256}" != "${OPENSSL_SHA256}" ]; then
    echo "OpenSSL archive checksum mismatch for ${OPENSSL_VERSION}" >&2
    exit 1
fi
if [ ! -d "${SOURCE_DIR}" ]; then
    tar -xzf "${ARCHIVE}" -C "${BUILD_ROOT}/src"
fi

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -v "${BUILD_ROOT}:${BUILD_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        set -e
        export PATH=/opt/devkitpro/devkitA64/bin:\$PATH
        cd '${SOURCE_DIR}'
        if git apply --check '${REPO_ROOT}/patches/openssl-3.0.16-switch-rng.patch' >/dev/null 2>&1; then
            git apply '${REPO_ROOT}/patches/openssl-3.0.16-switch-rng.patch'
        elif ! git apply --reverse --check '${REPO_ROOT}/patches/openssl-3.0.16-switch-rng.patch' >/dev/null 2>&1; then
            echo 'OpenSSL source does not match either side of the Switch RNG patch' >&2
            exit 1
        fi
        make clean >/dev/null 2>&1 || true
        CFLAGS='-DNO_SYSLOG -DNO_SYS_PARAM_H -D__SWITCH__ -I/opt/devkitpro/libnx/include' ./Configure \
            linux-generic64 no-shared no-tests no-engine no-dso \
            no-async no-sock no-ui-console --with-rand-seed=none \
            --cross-compile-prefix=aarch64-none-elf- \
            --prefix='${BUILD_ROOT}/install'
        make -s build_generated
        make -s -j\$(nproc) libcrypto.a libssl.a
    "

rm -rf "${BUILD_ROOT}/install/include" "${BUILD_ROOT}/install/lib"
mkdir -p "${BUILD_ROOT}/install/include" "${BUILD_ROOT}/install/lib"
cp -R "${SOURCE_DIR}/include/openssl" "${BUILD_ROOT}/install/include/"
cp "${SOURCE_DIR}/libcrypto.a" "${SOURCE_DIR}/libssl.a" "${BUILD_ROOT}/install/lib/"

echo "OpenSSL Switch prefix: ${BUILD_ROOT}/install"
