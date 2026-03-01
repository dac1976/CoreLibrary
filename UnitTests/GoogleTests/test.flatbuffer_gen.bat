@echo off
setlocal

set "CORELIB_TESTS_ROOT=%~dp0"
set "VCPKG_ROOT=%CORELIB_TESTS_ROOT%..\..\..\ThirdParty\vcpkg"
set "FLATC=%VCPKG_ROOT%\installed\x64-windows\tools\flatbuffers\flatc.exe"
set "FLATBUFFER_FILE=%CORELIB_TESTS_ROOT%test.fbs"

rem Strip trailing backslash from %~dp0
set "DIR_NO_SLASH=%CORELIB_TESTS_ROOT:~0,-1%"

echo CORELIB_TESTS_ROOT=%CORELIB_TESTS_ROOT%
echo DIR_NO_SLASH=%DIR_NO_SLASH%
echo FLATC=%FLATC%
echo FLATC version=%FLATC% --version
echo FLATBUFFER_FILE=%FLATBUFFER_FILE%

if not exist "%FLATC%" (
  echo ERROR: flatc not found: "%FLATC%"
  exit /b 1
)

if not exist "%FLATBUFFER_FILE%" (
  echo ERROR: flatbuffer schema file not found: "%FLATBUFFER_FILE%"
  exit /b 1
)

"%FLATC%" --cpp -o "%DIR_NO_SLASH%" --gen-object-api "%FLATBUFFER_FILE%"

endlocal