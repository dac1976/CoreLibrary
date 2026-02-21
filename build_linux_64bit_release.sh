#!/bin/bash

# Make sure you go to /Projvects/ThirdParty/libbacktrace and build and install libbacktrace.

set -e

export CORELIB_ROOT=/home/$USER/Projects/cpp/CoreLibrary
export CORELIB_BOOST_ROOT=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0
export CORELIB_BOOST_LIB=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0/lib64-msvc-14.3
export CORELIB_CEREAL_ROOT=${CORELIB_ROOT}/../ThirdParty/cereal/include
export CORELIB_LOKI_ROOT=${CORELIB_ROOT}/../ThirdParty/loki/include

# Tidy previous installation/build folders.
rm -rf build

rm -rf RelWithDebInfo
mkdir -p RelWithDebInfo

mkdir -p RelWithDebInfo/bin

OUTPUT_DIR="RelWithDebInfo"

# Create build files
cmake -Bbuild  -DCMAKE_INSTALL_PREFIX="${OUTPUT_DIR}"  -DBUILD_SHARED_LIBS=ON  -DCORELIB_USE_STD_FILESYSTEM=ON  -DCMAKE_BUILD_TYPE=RelWithDebInfo

# Perform build
cmake --build build -- -j$(nproc)

# Install
cmake --install build --prefix "${OUTPUT_DIR}"
