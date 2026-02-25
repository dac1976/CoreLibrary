#!/usr/bin/env bash
set -e

# Directory of this script (equivalent to %~dp0)
CORELIB_TESTS_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

VCPKG_ROOT="${CORELIB_TESTS_ROOT}/../../../ThirdParty/vcpkg"
PROTOC="${VCPKG_ROOT}/installed/x64-linux/tools/protobuf/protoc"
PROTO_FILE="${CORELIB_TESTS_ROOT}/test.proto"

DIR_NO_SLASH="${CORELIB_TESTS_ROOT}"

echo "CORELIB_TESTS_ROOT=${CORELIB_TESTS_ROOT}"
echo "DIR_NO_SLASH=${DIR_NO_SLASH}"
echo "PROTOC=${PROTOC}"
echo "PROTO_FILE=${PROTO_FILE}"

if [[ ! -f "${PROTOC}" ]]; then
    echo "ERROR: protoc not found: ${PROTOC}"
    exit 1
fi

if [[ ! -f "${PROTO_FILE}" ]]; then
    echo "ERROR: proto file not found: ${PROTO_FILE}"
    exit 1
fi

"${PROTOC}" \
  --proto_path="${DIR_NO_SLASH}" \
  --cpp_out="${DIR_NO_SLASH}" \
  "${PROTO_FILE}"