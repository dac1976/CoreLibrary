@echo off
setlocal

set "CORELIB_TESTS_ROOT=%~dp0"
set "VCPKG_ROOT=%CORELIB_TESTS_ROOT%..\..\..\ThirdParty\vcpkg"
set "PROTOC=%VCPKG_ROOT%\installed\x64-windows\tools\protobuf\protoc.exe"
set "PROTO_FILE=%CORELIB_TESTS_ROOT%test.proto"

rem Strip trailing backslash from %~dp0
set "DIR_NO_SLASH=%CORELIB_TESTS_ROOT:~0,-1%"

echo CORELIB_TESTS_ROOT=%CORELIB_TESTS_ROOT%
echo DIR_NO_SLASH=%DIR_NO_SLASH%
echo PROTOC=%PROTOC%
echo PROTO_FILE=%PROTO_FILE%

if not exist "%PROTOC%" (
  echo ERROR: protoc not found: "%PROTOC%"
  exit /b 1
)

if not exist "%PROTO_FILE%" (
  echo ERROR: proto file not found: "%PROTO_FILE%"
  exit /b 1
)

"%PROTOC%" ^
  --proto_path="%DIR_NO_SLASH%" ^
  --cpp_out="%DIR_NO_SLASH%" ^
  "%PROTO_FILE%"

endlocal