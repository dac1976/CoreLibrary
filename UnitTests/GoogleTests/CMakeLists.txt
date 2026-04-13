cmake_minimum_required(VERSION 3.16)

project(CoreLibraryUnitTests)

# GoogleTest requires at least C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Custom CMake toggle:
# -DCORELIB_USE_STD_FILESYSTEM=ON  -> (Default) use <filesystem>
# -DCORELIB_USE_STD_FILESYSTEM=OFF -> use <boost/filesystem.hpp>
option(CORELIB_USE_STD_FILESYSTEM "Use <filesystem> instead of <boost/filesystem.hpp>" ON)

option(CORELIB_DISABLE_ASIO_TESTS "Disable ASIO tests" ON)
option(CORELIB_DISABLE_CSVGRID_TESTS "Disable CSV grid tests" ON)
option(CORELIB_DISABLE_DEBUGLOG_TESTS "Disable debug log tests" ON)
option(CORELIB_DISABLE_FILEUTILS_TESTS "Disable file utils tests" ON)
option(CORELIB_DISABLE_FLOAT_COMPARE_TESTS "Disable float compare tests" ON)
option(CORELIB_DISABLE_SORTING_TESTS "Disable sorting tests" ON)
option(CORELIB_DISABLE_GPROTOBUF_TESTS "Disable Google protobuf tests" ON)
option(CORELIB_DISABLE_FLATBUFFER_TESTS "Disable Google flatbuffer tests" ON)
option(CORELIB_DISABLE_INIFILE_TESTS "Disable INI file tests" ON)
option(CORELIB_DISABLE_SERIALIZATION_TESTS "Disable serialization tests" ON)
option(CORELIB_DISABLE_STRINGUTILS_TESTS "Disable string utils tests" ON)
option(CORELIB_DISABLE_THREADS_TESTS "Disable threads tests" ON)
option(CORELIB_DISABLE_MANAGED_SINGLETON_TESTS "Disable managed singleton tests" ON)

# Local env vars for this CMakelists file, must be set before calling CMake configure
# and as these are cached will be rememebr until the CMake cache is cleared.
#
# Path that is parent to the /boost include directory.
set(CORELIB_BOOST_ROOT $ENV{CORELIB_BOOST_ROOT} CACHE PATH "Boost root")

# Path to the correct Boost lib folder for your compiler and platform.
set(CORELIB_BOOST_LIB $ENV{CORELIB_BOOST_LIB} CACHE PATH "Boost libs")

# Name modifier for boost libs.
set(CORELIB_BOOST_LIB_NAME_STUB $ENV{CORELIB_BOOST_LIB_NAME_STUB} CACHE PATH "Boost lib name stub")

# Name modifier for boost libs in debug mode.
set(CORELIB_BOOST_LIB_NAME_STUB_D $ENV{CORELIB_BOOST_LIB_NAME_STUB_D} CACHE PATH "Boost lib name stub (debug)")

# Check required paths are configured and exist on disk.
foreach(v CORELIB_BOOST_ROOT CORELIB_BOOST_LIB)
  if(NOT DEFINED ${v} OR "${${v}}" STREQUAL "")
    message(FATAL_ERROR "${v} is not set. Set env var ${v} or pass -D${v}=...")
  endif()

  if(NOT IS_DIRECTORY "${${v}}")
    message(FATAL_ERROR "${v} does not exist or is not a directory: '${${v}}'")
  endif()
endforeach()

# Find packages (safe on all platforms)
find_package(Threads REQUIRED)

# Protobuf packages.
if(NOT CORELIB_DISABLE_GPROTOBUF_TESTS)
  find_package(Protobuf CONFIG REQUIRED)
endif()

# flatbuffer packages.
if(NOT CORELIB_DISABLE_FLATBUFFER_TESTS)
  find_package(flatbuffers CONFIG REQUIRED)
endif()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  CoreLibraryUnitTests
  ../../Source/Threads/DeadlineTimer.cpp
  ../../Source/Threads/EventThread.cpp
  ../../Source/Threads/SyncEvent.cpp
  ../../Source/Threads/ThreadBase.cpp
  ../../Source/Threads/ThreadGroup.cpp
  ../../Source/Threads/ThreadPriority.cpp
  ../../Source/Threads/ThreadRunner.cpp
  ../../Source/StringUtils/StringUtils.cpp
  ../../Source/IniFile/IniFileLines.cpp
  ../../Source/IniFile/IniFileSectionDetails.cpp
  ../../Source/IniFile/IniFile.cpp
  ../../Source/FileUtils/FileUtils.cpp
  ../../Source/DebugLog/DebugLog.cpp
  ../../Source/DebugLog/DebugLogSingleton.cpp
  ../../Source/CsvGrid/CsvGridCell.cpp
  ../../Source/CsvGrid/CsvGridCellDouble.cpp
  ../../Source/Serialization/SerializeToVector.cpp
  ../../Source/Asio/AsioDefines.cpp
  ../../Source/Asio/IoContextThreadGroup.cpp
  ../../Source/Asio/MessageUtils.cpp
  ../../Source/Asio/MulticastReceiver.cpp
  ../../Source/Asio/MulticastSender.cpp
  ../../Source/Asio/NetworkUtils.cpp
  ../../Source/Asio/Ping.cpp
  ../../Source/Asio/SerialPort.cpp
  ../../Source/Asio/SimpleMulticastReceiver.cpp
  ../../Source/Asio/SimpleMulticastSender.cpp
  ../../Source/Asio/SimpleTcpClient.cpp
  ../../Source/Asio/SimpleTcpClientList.cpp
  ../../Source/Asio/SimpleTcpServer.cpp
  ../../Source/Asio/SimpleUdpReceiver.cpp
  ../../Source/Asio/SimpleUdpSender.cpp
  ../../Source/Asio/TcpClient.cpp
  ../../Source/Asio/TcpClientList.cpp
  ../../Source/Asio/TcpConnection.cpp
  ../../Source/Asio/TcpConnections.cpp
  ../../Source/Asio/TcpServer.cpp
  ../../Source/Asio/UdpReceiver.cpp
  ../../Source/Asio/UdpSender.cpp
  test.pb.cc
  tst_ManagedSingleton.cpp
  tst_AsioTest.cpp
  tst_CsvGridTest.cpp
  tst_DebugLogTest.cpp
  tst_FileUtilsTest.cpp
  tst_float_compare.cpp
  tst_GenericSortingTest.cpp
  tst_GoogleProtoBufTest.cpp
  $<$<NOT:$<BOOL:${CORELIB_DISABLE_FLATBUFFER_TESTS}>>:tst_GoogleFlatBuffersTest.cpp>
  tst_IniFileTest.cpp
  tst_SerializationUtilsTest.cpp
  tst_StringUtilsTest.cpp
  tst_ThreadsTest.cpp
)

target_compile_definitions(CoreLibraryUnitTests PRIVATE
  CORE_LIBRARY_LIB
  $<$<BOOL:${CORELIB_DISABLE_ASIO_TESTS}>:DISABLE_ASIO_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_CSVGRID_TESTS}>:DISABLE_CSVGRID_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_DEBUGLOG_TESTS}>:DISABLE_DEBUGLOG_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_FILEUTILS_TESTS}>:DISABLE_FILEUTILS_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_FLOAT_COMPARE_TESTS}>:DISABLE_FLOAT_COMPARE_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_SORTING_TESTS}>:DISABLE_SORTING_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_GPROTOBUF_TESTS}>:DISABLE_GPROTOBUF_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_INIFILE_TESTS}>:DISABLE_INIFILE_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_SERIALIZATION_TESTS}>:DISABLE_SERIALIZATION_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_STRINGUTILS_TESTS}>:DISABLE_STRINGUTILS_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_THREADS_TESTS}>:DISABLE_THREADS_TESTS>
  $<$<BOOL:${CORELIB_DISABLE_MANAGED_SINGLETON_TESTS}>:DISABLE_MANAGED_SINGLETON_TESTS>
)

target_compile_definitions(CoreLibraryUnitTests PUBLIC
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
  $<$<PLATFORM_ID:Windows>:WINVER=0x0A00>
  $<$<PLATFORM_ID:Windows>:NOMINMAX>
  $<$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>:USE_STD_FILESYSTEM>
  $<$<NOT:$<BOOL:${CORELIB_DISABLE_FLATBUFFER_TESTS}>>:USE_FLATBUFFERS>
  _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
  NOT_USING_BOOST_LOCALE
  BOOST_BIND_GLOBAL_PLACEHOLDERS
  BOOST_THREAD_USE_LIB
  LOKI_CLASS_LEVEL_THREADING
)

# Include Paths
target_include_directories(CoreLibraryUnitTests PUBLIC
  "${CORELIB_BOOST_ROOT}"
  ../../Include
  ../../Include/msgpack
)

if(MSVC)
  target_compile_options(CoreLibraryUnitTests PUBLIC
    /wd4251 /wd4275 /wd4100 /wd4068 /bigobj
  )
  target_compile_definitions(CoreLibraryUnitTests PUBLIC
    _CRT_SECURE_NO_WARNINGS
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(CoreLibraryUnitTests PUBLIC
    -Wno-unknown-pragmas
    -Wno-cast-function-type
    -Wno-deprecated-declarations
    -Wno-trigraphs
  )
endif()

# --- Generator-expression helpers ---
set(_CORELIB_WIN "$<PLATFORM_ID:Windows>")
set(_CORELIB_LINUX "$<PLATFORM_ID:Linux>")
set(_CORELIB_DBG "$<CONFIG:Debug>")
set(_CORELIB_REL "$<IN_LIST:$<CONFIG>,Release;RelWithDebInfo;MinSizeRel>")
set(_CORELIB_NEED_BOOST_FS "$<NOT:$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>>")

target_link_libraries(CoreLibraryUnitTests PRIVATE
  GTest::gtest_main
  $<$<NOT:$<BOOL:${CORELIB_DISABLE_GPROTOBUF_TESTS}>>:protobuf::libprotobuf>
  $<$<NOT:$<BOOL:${CORELIB_DISABLE_FLATBUFFER_TESTS}>>:flatbuffers::flatbuffers>

  # Linux
  $<${_CORELIB_LINUX}:${CORELIB_BOOST_LIB}/libboost_system.a>
  $<${_CORELIB_LINUX}:${CORELIB_BOOST_LIB}/libboost_thread.a>
  $<${_CORELIB_LINUX}:${CORELIB_BOOST_LIB}/libboost_locale.a>
  $<$<AND:${_CORELIB_LINUX},${_CORELIB_NEED_BOOST_FS}>:${CORELIB_BOOST_LIB}/libboost_filesystem.a>
  $<${_CORELIB_LINUX}:${CORELIB_BOOST_LIB}/libboost_program_options.a>
  $<${_CORELIB_LINUX}:Threads::Threads>

  # Windows
  $<$<PLATFORM_ID:Windows>:iphlpapi>

  # Windows Release-ish
  $<$<AND:${_CORELIB_WIN},${_CORELIB_REL}>:${CORELIB_BOOST_LIB}/libboost_thread-${CORELIB_BOOST_LIB_NAME_STUB}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_REL}>:${CORELIB_BOOST_LIB}/libboost_locale-${CORELIB_BOOST_LIB_NAME_STUB}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_REL},${_CORELIB_NEED_BOOST_FS}>:${CORELIB_BOOST_LIB}/libboost_filesystem-${CORELIB_BOOST_LIB_NAME_STUB}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_REL}>:${CORELIB_BOOST_LIB}/libboost_program_options-${CORELIB_BOOST_LIB_NAME_STUB}.lib>

  # Windows Debug
  $<$<AND:${_CORELIB_WIN},${_CORELIB_DBG}>:${CORELIB_BOOST_LIB}/libboost_thread-${CORELIB_BOOST_LIB_NAME_STUB_D}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_DBG}>:${CORELIB_BOOST_LIB}/libboost_locale-${CORELIB_BOOST_LIB_NAME_STUB_D}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_DBG},${_CORELIB_NEED_BOOST_FS}>:${CORELIB_BOOST_LIB}/libboost_filesystem-${CORELIB_BOOST_LIB_NAME_STUB_D}.lib>
  $<$<AND:${_CORELIB_WIN},${_CORELIB_DBG}>:${CORELIB_BOOST_LIB}/libboost_program_options-${CORELIB_BOOST_LIB_NAME_STUB_D}.lib>
)

include(GoogleTest)
gtest_discover_tests(CoreLibraryUnitTests)
