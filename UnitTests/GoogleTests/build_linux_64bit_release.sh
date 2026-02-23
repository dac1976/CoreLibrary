#!/bin/bash

# Make sure you go to /Projvects/ThirdParty/libbacktrace and build and install libbacktrace.

set -e

export CORELIB_ROOT=/home/$USER/Projects/cpp/CoreLibrary
export CORELIB_BOOST_ROOT=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0
export CORELIB_BOOST_LIB=${CORELIB_ROOT}/../ThirdParty/boost_1_90_0/lib64-msvc-14.3
# Not nmeeded fo Linux build
export CORELIB_BOOST_LIB_NAME_STUB=
export CORELIB_BOOST_LIB_NAME_STUB_D=

# Tidy previous installation/build folders.
rm -rf build

# Create build files
cmake -S . -B build -DCORELIB_USE_STD_FILESYSTEM=OFF -DCMAKE_BUILD_TYPE=Release

# Perform build
cmake --build build

cd build

ctest