@echo off

setlocal enabledelayedexpansion

SET "CORELIB_ROOT=%~dp0"
SET "CORELIB_BOOST_ROOT=%CORELIB_ROOT%../../../ThirdParty/boost_1_90_0"
SET "CORELIB_BOOST_LIB=%CORELIB_ROOT%../../../ThirdParty/boost_1_90_0/lib64-msvc-14.3"
SET "CORELIB_BOOST_LIB_NAME_STUB=vc143-mt-x64-1_90"
SET "CORELIB_BOOST_LIB_NAME_STUB_D=vc143-mt-gd-x64-1_90"

REM Setup environment.
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Tidy previous installation/build folders.
del /s /q build\*.*
rmdir /s /q build

REM Create build files
cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build -DCORELIB_USE_STD_FILESYSTEM=ON -DCMAKE_BUILD_TYPE=Release

REM Perform build
cmake --build build

cd build

ctest

endlocal
