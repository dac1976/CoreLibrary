@echo off

call test.proto_gen.bat

setlocal enabledelayedexpansion

SET "CORELIB_ROOT=%~dp0"
SET "CORELIB_BOOST_ROOT=%CORELIB_ROOT%../../../ThirdParty/boost_1_90_0"
SET "CORELIB_BOOST_LIB=%CORELIB_ROOT%../../../ThirdParty/boost_1_90_0/lib64-msvc-14.3"
SET "CORELIB_BOOST_LIB_NAME_STUB=vc143-mt-x64-1_90"
SET "CORELIB_BOOST_LIB_NAME_STUB_D=vc143-mt-gd-x64-1_90"
SET "CORELIB_VCPKG_CMAKE_PATH=%CORELIB_ROOT%../../../ThirdParty/vcpkg/scripts/buildsystems/vcpkg.cmake"

REM Setup environment.
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Tidy previous installation/build folders.
del /s /q build\*.*
rmdir /s /q build

REM Create build files
REM To disable specific tests specify any of the following.
REM E.g. CORELIB_DISABLE_ASIO_TESTS=ON, disables ASIO tests, CORELIB_DISABLE_ASIO_TESTS=OFF enables ASIO tests
cmake -G "Visual Studio 17 2022" -A x64 -T v143 -S . -B build -DCMAKE_TOOLCHAIN_FILE="%CORELIB_VCPKG_CMAKE_PATH%" -DVCPKG_TARGET_TRIPLET=x64-windows -DCORELIB_USE_STD_FILESYSTEM=ON -DCORELIB_DISABLE_ASIO_TESTS=OFF -DCORELIB_DISABLE_CSVGRID_TESTS=OFF -DCORELIB_DISABLE_DEBUGLOG_TESTS=OFF -DCORELIB_DISABLE_FILEUTILS_TESTS=OFF -DCORELIB_DISABLE_FLOAT_COMPARE_TESTS=OFF -DCORELIB_DISABLE_SORTING_TESTS=OFF -DCORELIB_DISABLE_GPROTOBUF_TESTS=OFF -DCORELIB_DISABLE_INIFILE_TESTS=OFF -DCORELIB_DISABLE_SERIALIZATIOFF_TESTS=OFF -DCORELIB_DISABLE_STRINGUTILS_TESTS=OFF -DCORELIB_DISABLE_THREADS_TESTS=OFF -DCORELIB_DISABLE_FLATBUFFER_TESTS=OFF

REM Perform build
cmake --build build --config Debug

cd build

ctest -C Debug

endlocal
