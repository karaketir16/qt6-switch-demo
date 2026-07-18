#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
. "${REPO_ROOT}/scripts/dotnet-env.sh"
. "${REPO_ROOT}/scripts/devkit-image.sh"

BUILD_DIR="${1:-${REPO_ROOT}/build/qtbase-switch-tests}"
BUILD_DIR="$(cd "${BUILD_DIR}" && pwd)"
RYUBING_BIN="${REPO_ROOT}/third_party/ryubing/src/Ryujinx/bin/Release/net10.0/Ryujinx"
RYUBING_SDCARD="${RYUBING_SDCARD:-${HOME}/Library/Application Support/Ryujinx/sdcard}"
NACP="${REPO_ROOT}/demo/qt-module-test/qt6-switch-module-test.nacp"
NRO_DIR="${BUILD_DIR}/nro-tests"
RESULT_DIR="${BUILD_DIR}/corelib-test-results"
SUMMARY_FILE="${RESULT_DIR}/summary.tsv"
GUEST_RESULT="${RYUBING_SDCARD}/qt6-switch-qtest-result.txt"
TIMEOUT="${QTBASE_TEST_TIMEOUT:-60}"

test -x "${RYUBING_BIN}" || { echo "Build Ryubing first: ./scripts/build-ryubing.sh" >&2; exit 1; }
test -x "${DOTNET_BIN}" || { echo "Set DOTNET to the .NET 10 SDK executable." >&2; exit 1; }
test -f "${NACP}" || { echo "Missing NACP metadata: ${NACP}" >&2; exit 1; }
command -v jq >/dev/null || { echo "jq is required to enumerate CTest targets." >&2; exit 1; }

mkdir -p "${NRO_DIR}" "${RESULT_DIR}" "${RYUBING_SDCARD}"
printf 'status\ttest\tpassed\tfailed\tskipped\treport\n' > "${SUMMARY_FILE}"

labels=()
while IFS= read -r label; do
    labels+=("${label}")
done < <(
    ctest --test-dir "${BUILD_DIR}" --show-only=json-v1 |
        jq -r '.tests[].properties[]? | select(.name == "LABELS") | .value[]' |
        sort -u | grep '^tests/auto/corelib/'
)

passed=0
failed=0
skipped=0
for label in "${labels[@]}"; do
    executable="${BUILD_DIR}/${label}"
    safe_name="${label//\//__}"
    nro="${NRO_DIR}/${safe_name}.nro"
    result="${RESULT_DIR}/${safe_name}.txt"
    stdout="${RESULT_DIR}/${safe_name}.stdout"
    stderr="${RESULT_DIR}/${safe_name}.stderr"

    if [ ! -x "${executable}" ]; then
        printf 'SKIP (not built): %s\n' "${label}" | tee "${result}"
        printf 'SKIP\t%s\t0\t0\t0\t%s\n' "${label}" "${result}" >> "${SUMMARY_FILE}"
        skipped=$((skipped + 1))
        continue
    fi

    docker run --rm \
        -v "${REPO_ROOT}:${REPO_ROOT}" \
        -w "${BUILD_DIR}" \
        "${DEVKITA64_IMAGE}" \
        elf2nro "${label}" "nro-tests/${safe_name}.nro" "--nacp=${NACP}"

    rm -f "${GUEST_RESULT}"
    "${RYUBING_BIN}" "${nro}" >"${stdout}" 2>"${stderr}" &
    ryubing_pid=$!
    completed=0
    for _ in $(seq 1 "${TIMEOUT}"); do
        if [ -f "${GUEST_RESULT}" ] && grep -q '^\*\*\*\*\*\*\*\*\* Finished testing of ' "${GUEST_RESULT}"; then
            completed=1
            break
        fi
        if ! kill -0 "${ryubing_pid}" 2>/dev/null; then
            break
        fi
        sleep 1
    done
    kill "${ryubing_pid}" 2>/dev/null || true
    wait "${ryubing_pid}" 2>/dev/null || true

    if [ "${completed}" = 1 ]; then
        cp "${GUEST_RESULT}" "${result}"
        if grep -Eq '^Totals: .* 0 failed,' "${result}"; then
            printf 'PASS: %s\n' "${label}"
            totals=$(grep '^Totals: ' "${result}" | tail -1)
            passed_cases=$(printf '%s\n' "${totals}" | sed -E 's/^Totals: ([0-9]+) passed, ([0-9]+) failed, ([0-9]+) skipped,.*/\1/')
            failed_cases=$(printf '%s\n' "${totals}" | sed -E 's/^Totals: ([0-9]+) passed, ([0-9]+) failed, ([0-9]+) skipped,.*/\2/')
            skipped_cases=$(printf '%s\n' "${totals}" | sed -E 's/^Totals: ([0-9]+) passed, ([0-9]+) failed, ([0-9]+) skipped,.*/\3/')
            printf 'PASS\t%s\t%s\t%s\t%s\t%s\n' "${label}" "${passed_cases}" "${failed_cases}" "${skipped_cases}" "${result}" >> "${SUMMARY_FILE}"
            passed=$((passed + 1))
        else
            printf 'FAIL: %s\n' "${label}"
            totals=$(grep '^Totals: ' "${result}" | tail -1 || true)
            printf 'FAIL\t%s\t%s\t%s\t%s\t%s\n' "${label}" "${totals:-no-report}" "" "" "${result}" >> "${SUMMARY_FILE}"
            failed=$((failed + 1))
        fi
    else
        printf 'FAIL (no complete QtTest report): %s\n' "${label}" | tee "${result}"
        printf 'FAIL\t%s\t0\t0\t0\t%s\n' "${label}" "${result}" >> "${SUMMARY_FILE}"
        failed=$((failed + 1))
    fi
done

printf 'QtBase Switch Corelib QtTest summary: %d passed, %d failed, %d skipped/not-built\n' \
    "${passed}" "${failed}" "${skipped}"
test "${failed}" -eq 0
