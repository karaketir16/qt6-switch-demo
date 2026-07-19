#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/devkit-image.sh"
BATCH_BUILD_DIR="${BATCH_BUILD_DIR:-${REPO_ROOT}/build/qt-switch-batch}"
OUTPUT_DIR="${OUTPUT_DIR:-${BATCH_BUILD_DIR}/nro}"
NACP="${NACP:-${REPO_ROOT}/demo/qt-module-test/qt6-switch-module-test.nacp}"

test -f "${NACP}"
mkdir -p "${OUTPUT_DIR}"

batch_targets=("${BATCH_BUILD_DIR}"/*_tests_batch)
if [ ! -x "${batch_targets[0]}" ]; then
    echo "No batch executables in ${BATCH_BUILD_DIR}" >&2
    exit 1
fi

docker run --rm \
    -v "${REPO_ROOT}:${REPO_ROOT}" \
    -w "${OUTPUT_DIR}" \
    "${DEVKITA64_IMAGE}" \
    bash -lc "$(for target in "${batch_targets[@]}"; do
        name="$(basename "${target}")"
        printf "elf2nro '%s' '%s/%s.nro' --nacp='%s'\n" \
            "${target}" "${OUTPUT_DIR}" "${name}" "${NACP}"
    done)"

echo "Batch NROs: ${OUTPUT_DIR}"
