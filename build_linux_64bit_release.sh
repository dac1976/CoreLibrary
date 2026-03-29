#!/bin/bash

# Make sure you go to /Projvects/ThirdParty/libbacktrace and build and install libbacktrace.

set -e

export CORELIB_ROOT=/home/$USER/Projects/cpp/CoreLibrary
export CORELIB_BOOST_ROOT=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0
export CORELIB_BOOST_LIB=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0/lib
# Not nmeeded fo Linux build
export CORELIB_BOOST_LIB_NAME_STUB=
export CORELIB_BOOST_LIB_NAME_STUB_D=
export CORELIB_VCPKG_CMAKE_PATH=${CORELIB_ROOT}/../ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake

# Tidy previous installation/build folders.
rm -rf build

rm -rf RelWithDebInfo
mkdir -p RelWithDebInfo

mkdir -p RelWithDebInfo/static
mkdir -p RelWithDebInfo/dynamic

# Create build files
cmake -Bbuild  -DCMAKE_INSTALL_PREFIX="./RelWithDebInfo/dynamic" -DCMAKE_TOOLCHAIN_FILE="${CORELIB_VCPKG_CMAKE_PATH}" -DBUILD_SHARED_LIBS=ON -DCORELIB_USE_FLATBUFFERS=ON -DCORELIB_USE_STD_FILESYSTEM=ON  -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Perform build
cmake --build build -- -j$(nproc)

# Install
cp ./build/libCoreLibrary.so ./RelWithDebInfo/dynamic

rm -rf build

# Create build files
cmake -Bbuild  -DCMAKE_INSTALL_PREFIX="./RelWithDebInfo/static"  -DBUILD_SHARED_LIBS=OFF  -DCORELIB_USE_STD_FILESYSTEM=ON  -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Perform build
cmake --build build -- -j$(nproc)

# Install
cp ./build/libCoreLibrary.a ./RelWithDebInfo/static

rm -rf build
