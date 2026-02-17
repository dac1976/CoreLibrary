cmake_minimum_required(VERSION 3.16)

project(CoreLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Standard CMake toggle:
#   -DBUILD_SHARED_LIBS=ON  -> SHARED
#   -DBUILD_SHARED_LIBS=OFF -> STATIC
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Custom CMake toggle:
#   -DCORELIB_USE_STD_FILESYSTEM=ON  -> (Default) use <filesystem>
#   -DCORELIB_USE_STD_FILESYSTEM=OFF -> use <boost/filesystem.hpp>
option(CORELIB_USE_STD_FILESYSTEM "Use <filesystem> instead of <boost/filesystem.hpp>" ON)

# Custom CMake toggle:
#   -DCORELIB_SOCKET_DEBUG=ON  -> Extra debug to log.
#   -DCORELIB_SOCKET_DEBUG=OFF -> (Default) No extra debug to log/
option(CORELIB_SOCKET_DEBUG "Use extra debug output from Asio classes" OFF)


# Local env vars for this CMakelists file, must be set before calling CMake configure
# and as these are cached will be rememebr until the CMake cache is cleared.
#
# Path that is parent to the /boost include directory.
set(CORELIB_BOOST_ROOT $ENV{CORELIB_BOOST_ROOT} CACHE PATH "Boost root")
# Path to the correct Boost lib folder for your compiler and platform.
set(CORELIB_BOOST_LIB $ENV{CORELIB_BOOST_LIB} CACHE PATH "Boost libs")
# Path (inclusive) for the Cereal /include directory.
set(CORELIB_CEREAL_ROOT $ENV{CORELIB_CEREAL_ROOT} CACHE PATH "Cereal root")
# Path (inclusive) for the Loki /include directory.
set(CORELIB_LOKI_ROOT $ENV{CORELIB_LOKI_ROOT} CACHE PATH "Loki root")

# Check required paths are configured and exist on disk.
foreach(v CORELIB_BOOST_ROOT CORELIB_BOOST_LIB CORELIB_CEREAL_ROOT CORELIB_LOKI_ROOT)
  if(NOT DEFINED ${v} OR "${${v}}" STREQUAL "")
    message(FATAL_ERROR "${v} is not set. Set env var ${v} or pass -D${v}=...")
  endif()

  if(NOT IS_DIRECTORY "${${v}}")
    message(FATAL_ERROR "${v} does not exist or is not a directory: '${${v}}'")
  endif()
endforeach()



# Find packages
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  find_package(Threads REQUIRED)
endif()

# Define the module
add_library(CoreLibrary
  Source/Threads/DeadlineTime.cpp
  Source/Threads/EventThread.cpp
  Source/Threads/SyncEvent.cpp
  Source/Threads/ThreadBase.cpp
  Source/Threads/ThreadGroup.cpp
  Source/Threads/ThreadPriority.cpp
  Source/Threads/ThreadRunner.cpp
  Source/StringUtils/StringUtils.cpp
  Source/IniFile/IniFileLines.cpp
  Source/IniFile/IniFileSectionDetails.cpp
  Source/IniFile/IniFile.cpp
  Source/FileUtils/FileUtils.cpp
  Source/DebugLog/DebugLog.cpp
  Source/DebugLog/DebugLogSingleton.cpp
  Source/CsvGrid/CsvGridCell.cpp
  Source/CsvGrid/CsvGridCellDouble.cpp
  Source/Asio/AsioDefines.cpp
  Source/Asio/IoContextThreadGroup.cpp
  Source/Asio/MessageUtils.cpp
  Source/Asio/MulticastReceiver.cpp
  Source/Asio/MulticastSender.cpp
  Source/Asio/NetworkUtils.cpp
  Source/Asio/Ping.cpp
  Source/Asio/SerialPort.cpp
  Source/Asio/SimpleMulticastReceiver.cpp
  Source/Asio/SimpleMulticastSender.cpp
  Source/Asio/SimpleTcpClient.cpp
  Source/Asio/SimpleTcpClientList.cpp
  Source/Asio/SimpleTcpServer.cpp
  Source/Asio/SimpleUdpReceiver.cpp
  Source/Asio/SimpleUdpSender.cpp
  Source/Asio/TcpClient.cpp
  Source/Asio/TcpClientList.cpp
  Source/Asio/TcpConnection.cpp
  Source/Asio/TcpConnections.cpp
  Source/Asio/TcpServer.cpp
  Source/Asio/UdpReceiver.cpp
  Source/Asio/UdpSender.cpp
)

# Public preprocessor options
target_compile_definitions(CoreLibrary PRIVATE
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:CORE_LIBRARY_LIB=1>
  $<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:CORE_LIBRARY_DLL=1>
)

target_compile_definitions(CoreLibrary PUBLIC
  $<$<PLATFORM_ID:Windows>:WIN32_LEAN_AND_MEAN>
  $<$<PLATFORM_ID:Windows>:_WIN32_WINNT=0x0A00>
  $<$<PLATFORM_ID:Windows>:WINVER=0x0A00>
  $<$<PLATFORM_ID:Windows>:NOMINMAX>
  $<$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>:USE_STD_FILESYSTEM>
  $<$<BOOL:${CORELIB_SOCKET_DEBUG}>:USE_SOCKET_DEBUG>
  _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
  NOT_USING_BOOST_LOCALE
  BOOST_BIND_GLOBAL_PLACEHOLDERS
  BOOST_THREAD_USE_LIB
  LOKI_CLASS_LEVEL_THREADING
)

if(MSVC)
  target_compile_options(CoreLibrary PUBLIC 
    /wd4251 /wd4275 /wd4100 /wd4068 /bigobj
  )
  
  target_compile_definitions(CoreLibrary PUBLIC 
    _CRT_SECURE_NO_WARNINGS
  )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(CoreLibrary PUBLIC 
    -Wno-unknown-pragmas
	-Wno-cast-function-type
	-Wno-deprecated-declarations
	-Wno-trigraphs
  )
endif()

# Include Paths
target_include_directories(CoreLibrary PUBLIC
  "${CORELIB_BOOST_ROOT}"  
  "${CORELIB_LOKI_ROOT}/include"
  "${CORELIB_CEREAL_ROOT}/include"
  Include
)

# Keep the directories private
#target_link_directories(CoreLibrary PRIVATE
#  "${CORELIB_BOOST_LIB}"
#)

# Linking Dependencies publicly
target_link_libraries(CoreLibrary PUBLIC
  $<$<PLATFORM_ID:Linux>:${CORELIB_BOOST_LIB}/libboost_system.a>
  $<$<PLATFORM_ID:Linux>:${CORELIB_BOOST_LIB}/libboost_thread.a>
  $<$<PLATFORM_ID:Linux>:${CORELIB_BOOST_LIB}/libboost_locale.a>
  $<$<AND:$<PLATFORM_ID:Linux>,$<NOT:$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>>>:${CORELIB_BOOST_LIB}/libboost_filesystem.a>
  $<$<PLATFORM_ID:Linux>:${CORELIB_BOOST_LIB}/libboost_program_options.a>
  $<$<PLATFORM_ID:Linux>:Threads::Threads>    
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release;RelWithDebInfo;MinSizeRel>:${CORELIB_BOOST_LIB}/libboost_system-vc143-mt-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release;RelWithDebInfo;MinSizeRel>:${CORELIB_BOOST_LIB}/libboost_thread-vc143-mt-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release;RelWithDebInfo;MinSizeRel>:${CORELIB_BOOST_LIB}/libboost_locale-vc143-mt-x64-1_85.lib>>
  $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Release;RelWithDebInfo;MinSizeRel>,$<NOT:$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>>>:${CORELIB_BOOST_LIB}/libboost_filesystem-vc143-mt-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Release;RelWithDebInfo;MinSizeRel>:${CORELIB_BOOST_LIB}/libboost_program_options-vc143-mt-x64-1_85.lib>>	
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:${CORELIB_BOOST_LIB}/libboost_system-vc143-mt-gd-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:${CORELIB_BOOST_LIB}/libboost_thread-vc143-mt-gd-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:${CORELIB_BOOST_LIB}/libboost_locale-vc143-mt-gd-x64-1_85.lib>>
  $<$<AND:$<PLATFORM_ID:Windows>,$<CONFIG:Debug>,$<NOT:$<BOOL:${CORELIB_USE_STD_FILESYSTEM}>>>:${CORELIB_BOOST_LIB}/libboost_filesystem-vc143-mt-gd-x64-1_85.lib>>
  $<$<PLATFORM_ID:Windows>:$<$<CONFIG:Debug>:${CORELIB_BOOST_LIB}/libboost_program_options-vc143-mt-gd-x64-1_85.lib>>
)

