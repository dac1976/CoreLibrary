@echo off

setlocal enabledelayedexpansion

SET "CORELIB_ROOT=%~dp0"
SET "CORELIB_BOOST_ROOT=%CORELIB_ROOT%../ThirdParty/boost_1_90_0"
SET "CORELIB_BOOST_LIB=%CORELIB_ROOT%../ThirdParty/boost_1_90_0/lib64-msvc-14.3"
SET "CORELIB_CEREAL_ROOT=%CORELIB_ROOT%../ThirdParty/cereal/include"
SET "CORELIB_LOKI_ROOT=%CORELIB_ROOT%../ThirdParty/loki/include"

REM Setup environment.
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

REM Tidy previous installation/build folders.
del /s /q build\*.*
rmdir /s /q build

del /s /q bin\RelWithDebInfo\*.*
rmdir /s /q bin\RelWithDebInfo

mkdir bin\RelWithDebInfo

REM Create build files
cmake -G "Visual Studio 17 2022" -A x64 -T v143 -Bbuild -DBUILD_SHARED_LIBS=ON -DCORELIB_USE_STD_FILESYSTEM=ON -DCMAKE_INSTALL_PREFIX=./ -DCMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO="/DEBUG:FULL /OPT:REF /OPT:ICF"

REM Perform build
cmake --build build --config RelWithDebInfo --target ALL_BUILD --parallel

REM copy files to bin64 output folder
copy .\build\RelWithDebInfo\CoreLibrary.dll /B .\bin\RelWithDebInfo\CoreLibrary.dll
copy .\build\RelWithDebInfo\CoreLibrary.pdb /B .\bin\RelWithDebInfo\CoreLibrary.pdb

endlocal
