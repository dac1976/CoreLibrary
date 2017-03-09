#-------------------------------------------------
#
# Project created by QtCreator 2013-07-20T22:13:38
#
#-------------------------------------------------

#set version info for library
VERSION = 1.3.0

QT       -= gui

QMAKE_CXXFLAGS += -std=c++14

TARGET = CoreLibraryDll
TEMPLATE = lib
CONFIG += dll debug_and_release build_all
CONFIG += core_lib_settings
CONFIG += boost_libs

DEFINES += CORE_LIBRARY_DLL

unix {
	target.path = /usr/lib
	INSTALLS += target
}

CONFIG(debug, debug|release) {
  # TARGET = $$join(TARGET,,,d) # if compiling in debug mode, append a "d" to the application name
  DESTDIR = debug
} else {
  DESTDIR = release
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

HEADERS +=                                              \
	../../Include/Asio/AsioDefines.h                    \
    ../../Include/Asio/IoServiceThreadGroup.h           \
	../../Include/Asio/MessageUtils.h                   \
	../../Include/Asio/SimpleTcpClient.h                \
	../../Include/Asio/SimpleTcpServer.h                \
	../../Include/Asio/TcpClient.h                      \
	../../Include/Asio/TcpConnection.h                  \
	../../Include/Asio/TcpConnections.h                 \
	../../Include/Asio/TcpServer.h                      \
	../../Include/Asio/TcpTypedClient.h                 \
	../../Include/Asio/TcpTypedServer.h                 \
	../../Include/CsvGrid/CsvGridCell.h                 \
	../../Include/CsvGrid/CsvGridRow.h                  \
	../../Include/CsvGrid/CsvGridMain.h                 \
	../../Include/CsvGrid/CsvGrid.h                     \
	../../Include/DebugLog/DebugLog.h                   \
	../../Include/DebugLog/DebugLogSingleton.h          \
	../../Include/DebugLog/DebugLogging.h               \
	../../Include/Exceptions/CustomException.h          \
    ../../Include/FileUtils/FileUtils.h                 \
	../../Include/IniFile/IniFileLines.h                \
	../../Include/IniFile/IniFileSectionDetails.h       \
	../../Include/IniFile/IniFile.h                     \
	../../Include/Serialization/SerializationIncludes.h \
	../../Include/Serialization/SerializeToVector.h     \
	../../Include/Sorting/GenericSorting.h              \
	../../Include/StringUtils/StringUtils.h             \
	../../Include/Threads/BoundedBuffer.h               \
	../../Include/Threads/ConcurrentQueue.h             \
	../../Include/Threads/JoinThreads.h                 \
	../../Include/Threads/MessageQueueThread.h          \
	../../Include/Threads/SyncEvent.h                   \
	../../Include/Threads/ThreadBase.h                  \
	../../Include/Threads/ThreadGroup.h                 \
	../../Include/Asio/UdpSender.h                      \
	../../Include/Asio/UdpReceiver.h                    \
    ../../Include/Asio/MulticastReceiver.h              \
    ../../Include/Asio/MulticastSender.h                \
    ../../Include/Asio/SimpleMulticastReceiver.h        \
    ../../Include/Asio/SimpleMulticastSender.h          \
    ../../Include/Asio/MulticastTypedSender.h           \
	../../Include/Asio/UdpTypedSender.h                 \
    ../../Include/Asio/SimpleUdpSender.h                \
    ../../Include/Asio/SimpleUdpReceiver.h              \
    ../../Include/Platform/PlatformDefines.h            \
	../../Include/CoreLibraryDllGlobal.h

SOURCES +=                                                \
    ../../Source/Asio/AsioDefines.cpp                     \
	../../Source/Asio/IoServiceThreadGroup.cpp            \
	../../Source/Asio/MessageUtils.cpp                    \
	../../Source/Asio/SimpleTcpClient.cpp                 \
	../../Source/Asio/SimpleTcpServer.cpp                 \
	../../Source/Asio/TcpClient.cpp                       \
	../../Source/Asio/TcpConnection.cpp                   \
	../../Source/Asio/TcpConnections.cpp                  \
	../../Source/Asio/TcpServer.cpp                       \
	../../Source/CsvGrid/CsvGridCell.cpp                  \
	../../Source/CsvGrid/CsvGridRow.cpp                   \
	../../Source/CsvGrid/CsvGridMain.cpp                  \
	../../Source/DebugLog/DebugLog.cpp                    \
	../../Source/Exceptions/CustomException.cpp           \
    ../../Source/FileUtils/FileUtils.cpp                  \
	../../Source/IniFile/IniFileLines.cpp                 \
	../../Source/IniFile/IniFileSectionDetails.cpp        \
	../../Source/IniFile/IniFile.cpp                      \
	../../Source/Sorting/GenericSorting.cpp               \
	../../Source/StringUtils/StringUtils.cpp              \
	../../Source/Threads/ConcurrentQueue.cpp              \
	../../Source/Threads/MessageQueueThread.cpp           \
	../../Source/Threads/SyncEvent.cpp                    \
	../../Source/Threads/ThreadBase.cpp                   \
	../../Source/Threads/ThreadGroup.cpp                  \
	../../Source/Asio/UdpSender.cpp                       \
	../../Source/Asio/UdpReceiver.cpp                     \
    ../../Source/Asio/MulticastReceiver.cpp               \
    ../../Source/Asio/MulticastSender.cpp                 \
    ../../Source/Asio/SimpleUdpSender.cpp                 \
    ../../Source/Asio/SimpleUdpReceiver.cpp               \
    ../../Source/Asio/SimpleMulticastReceiver.cpp         \
    ../../Source/Asio/SimpleMulticastSender.cpp          



