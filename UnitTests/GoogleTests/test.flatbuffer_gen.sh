#!/usr/bin/env bash
set -e

# Directory of this script (equivalent to %~dp0)
CORELIB_TESTS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

VCPKG_ROOT="${CORELIB_TESTS_ROOT}/../../../ThirdParty/vcpkg"
FLATC="${VCPKG_ROOT}/installed/x64-linux/tools/flatbuffers/flatc"
FLATBUFFER_FILE="${CORELIB_TESTS_ROOT}/test.fbs"

DIR_NO_SLASH="${CORELIB_TESTS_ROOT}"

echo "CORELIB_TESTS_ROOT=${CORELIB_TESTS_ROOT}"
echo "DIR_NO_SLASH=${DIR_NO_SLASH}"
echo "FLATC=${FLATC}"
echo "FLATBUFFER_FILE=${FLATBUFFER_FILE}"

if [[ ! -f "${FLATC}" ]]; then
    echo "ERROR: flatc not found: ${PROTOC}"
    exit 1
fi

if [[ ! -f "${FLATBUFFER_FILE}" ]]; then
    echo "ERROR: flatbuffer schema file not found: ${FLATBUFFER_FILE}"
    exit 1
fi

"%FLATC%" --cpp -o "%DIR_NO_SLASH%" --gen-object-api "%FLATBUFFER_FILE%"