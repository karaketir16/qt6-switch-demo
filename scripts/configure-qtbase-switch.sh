#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
QTBASE_DIR="${1:-${REPO_ROOT}/third_party/qtbase}"
BUILD_DIR="${2:-${REPO_ROOT}/build/qtbase-switch}"
TOOLCHAIN_FILE="${3:-${REPO_ROOT}/extras/toolchain-switch.cmake}"
QT_HOST_PATH_VALUE="${QT_HOST_PATH:-${4:-${REPO_ROOT}/build/qtbase-host}}"
QT_FEATURE_WIDGETS_VALUE="${QT_FEATURE_WIDGETS:-${5:-ON}}"
OPENSSL_ROOT_DIR_VALUE="${OPENSSL_ROOT_DIR:-${REPO_ROOT}/build/openssl-switch/install}"

if [ -z "${QT_HOST_PATH_VALUE}" ]; then
    echo "QT_HOST_PATH is required for Qt cross compilation." >&2
    echo "Example: QT_HOST_PATH=${REPO_ROOT}/build/qtbase-host ./configure-qtbase-switch.sh" >&2
    exit 1
fi
if [ ! -x "${QT_HOST_PATH_VALUE}/libexec/moc" ] && [ ! -x "${QT_HOST_PATH_VALUE}/bin/moc" ]; then
    echo "Missing QtBase host tool: moc (${QT_HOST_PATH_VALUE})" >&2
    exit 1
fi

mkdir -p "${BUILD_DIR}"

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${REPO_ROOT}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "
        cmake --fresh -S '${QTBASE_DIR}' -B '${BUILD_DIR}' -GNinja \
            -DQT_QMAKE_TARGET_MKSPEC=devices/switch-aarch64-libnx-g++ \
            -DSWITCH=ON \
            -DCMAKE_TOOLCHAIN_FILE='${TOOLCHAIN_FILE}' \
            -DQT_HOST_PATH='${QT_HOST_PATH_VALUE}' \
            -DCMAKE_BUILD_TYPE=Release \
            -DBUILD_SHARED_LIBS=OFF \
            -DQT_BUILD_EXAMPLES=OFF \
            -DQT_BUILD_TESTS=OFF \
            -DFEATURE_testlib=OFF \
            -DFEATURE_dbus=OFF \
            -DFEATURE_process=OFF \
            -DFEATURE_systemsemaphore=OFF \
            -DFEATURE_gui=ON \
            -DFEATURE_localserver=OFF \
            -DFEATURE_widgets='${QT_FEATURE_WIDGETS_VALUE}' \
            -DINPUT_openssl=linked \
            -DOPENSSL_ROOT_DIR='${OPENSSL_ROOT_DIR_VALUE}' \
            -DOPENSSL_INCLUDE_DIR='${OPENSSL_ROOT_DIR_VALUE}/include' \
            -DOPENSSL_SSL_LIBRARY='${OPENSSL_ROOT_DIR_VALUE}/lib/libssl.a' \
            -DOPENSSL_CRYPTO_LIBRARY='${OPENSSL_ROOT_DIR_VALUE}/lib/libcrypto.a'
    "

echo
echo "Qt Switch configuration completed."
echo "Repository root: ${REPO_ROOT}"
echo "Build directory: ${BUILD_DIR}"
