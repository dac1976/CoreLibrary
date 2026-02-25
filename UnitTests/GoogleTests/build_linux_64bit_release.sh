#!/bin/bash

bash test.proto_gen.sh

# Make sure you go to /Projvects/ThirdParty/libbacktrace and build and install libbacktrace.

set -e

export CORELIB_ROOT=/home/$USER/Projects/cpp/CoreLibrary
export CORELIB_BOOST_ROOT=${CORELIB_ROOT}/../../../ThirdParty/boost_1_90_0
export CORELIB_BOOST_LIB=${CORELIB_ROOT}/../../../ThirdParty/boost_1_90_0/lib64-msvc-14.3
# Not nmeeded fo Linux build
export CORELIB_BOOST_LIB_NAME_STUB=
export CORELIB_BOOST_LIB_NAME_STUB_D=
export CORELIB_VCPKG_CMAKE_PATH=${CORELIB_ROOT}/../../../ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake

# Tidy previous installation/build folders.
rm -rf build

# E.g. CORELIB_DISABLE_ASIO_TESTS=ON, disables ASIO tests, CORELIB_DISABLE_ASIO_TESTS=OFF enables ASIO tests
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE="%CORELIB_VCPKG_CMAKE_PATH%" -DVCPKG_TARGET_TRIPLET=x64-windows -DCORELIB_USE_STD_FILESYSTEM=ON -DCORELIB_DISABLE_ASIO_TESTS=OFF -DCORELIB_DISABLE_CSVGRID_TESTS=OFF -DCORELIB_DISABLE_DEBUGLOG_TESTS=OFF -DCORELIB_DISABLE_FILEUTILS_TESTS=OFF -DCORELIB_DISABLE_FLOAT_COMPARE_TESTS=OFF -DCORELIB_DISABLE_SORTING_TESTS=OFF -DCORELIB_DISABLE_GPROTOBUF_TESTS=OFF -DCORELIB_DISABLE_INIFILE_TESTS=OFF -DCORELIB_DISABLE_SERIALIZATIOFF_TESTS=OFF -DCORELIB_DISABLE_STRINGUTILS_TESTS=OFF -DCORELIB_DISABLE_THREADS_TESTS=OFF

# Perform build
cmake --build build --config Release

cd build

ctest -C Release