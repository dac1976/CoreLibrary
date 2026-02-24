#!/bin/bash

# Make sure you go to /Projvects/ThirdParty/libbacktrace and build and install libbacktrace.

set -e

export CORELIB_ROOT=/home/$USER/Projects/cpp/CoreLibrary
export CORELIB_BOOST_ROOT=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0
export CORELIB_BOOST_LIB=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0/lib64-msvc-14.3
# Not nmeeded fo Linux build
export CORELIB_BOOST_LIB_NAME_STUB=
export CORELIB_BOOST_LIB_NAME_STUB_D=
export CORELIB_PROTOBUF_ROOT=%CORELIB_ROOT%../../../ThirdParty/grpc/grpc_gcc11.4_v1.78.1/third_party/protobuf/src
export CORELIB_PROTOBUF_LIB=%CORELIB_ROOT%../../../ThirdParty/grpc/grpc_gcc11.4_v1.78.1/cmake/build/third_party/protobuf/Debug

# Tidy previous installation/build folders.
rm -rf build

# Create build files
# E.g. CORELIB_DISABLE_ASIO_SERIAL_TESTS=ON, disables ASIO Serial tests, =OFF enables asio serial tests
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCORELIB_USE_STD_FILESYSTEM=ON -DCORELIB_DISABLE_ASIO_TESTS=OFF -DCORELIB_DISABLE_CSVGRID_TESTS=OFF -DCORELIB_DISABLE_DEBUGLOG_TESTS=OFF -DCORELIB_DISABLE_FILEUTILS_TESTS=OFF -DCORELIB_DISABLE_FLOAT_COMPARE_TESTS=OFF -DCORELIB_DISABLE_SORTING_TESTS=OFF -DCORELIB_DISABLE_GPROTOBUF_TESTS=OFF -DCORELIB_DISABLE_INIFILE_TESTS=OFF -DCORELIB_DISABLE_SERIALIZATION_TESTS=OFF -DCORELIB_DISABLE_STRINGUTILS_TESTS=OFF 

# Perform build
cmake --build build

cd build

ctest