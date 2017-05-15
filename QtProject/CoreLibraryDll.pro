#-------------------------------------------------
#
# Project created by QtCreator 2013-07-20T22:13:38
#
#-------------------------------------------------

QT       -= gui

TARGET = CoreLibraryDll
TEMPLATE = lib
CONFIG += dll debug_and_release build_all
CONFIG += core_lib_settings
CONFIG += boost_libs

DEFINES += CORE_LIBRARY_DLL

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

# On Windows we do this, assumes we'll be using MS VC 2015.
win32 {
    # Set version info of our dll.
    RC_FILE=CoreLibraryDll_resource.rc

    # Due to exporting from DLL we might get suprious warnings of
    # type 4251, 4275 and 4100 so disable them.
    QMAKE_CXXFLAGS += /wd4251 /wd4275 /wd4100
    DEFINES += _CRT_SECURE_NO_WARNINGS=1

    # Set binary's output folder.
    # This is for x86 builds.
    !contains(QMAKE_HOST.arch, x86_64) {
        CONFIG(debug, debug|release) {
          DESTDIR = debug/x86
        } else {
          DESTDIR = release/x86
        }
    }
    # This is for x64 builds.
    else {
        CONFIG(debug, debug|release) {
          DESTDIR = debug/x64
        } else {
          DESTDIR = release/x64
        }
    }
}
# On non-windows, assumed to beLinux, we do ths.
else {
    # Make sure we enable C++14 support.
    QMAKE_CXXFLAGS += -std=c++14

    # Set version info for library.
    VERSION = 1.4.1

    # Set binary's output folder.
    CONFIG(debug, debug|release) {
      DESTDIR = debug
    } else {
      DESTDIR = release
    }
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

HEADERS +=                                           \
	../Include/Asio/AsioDefines.h                    \
    ../Include/Asio/IoServiceThreadGroup.h           \
	../Include/Asio/MessageUtils.h                   \
	../Include/Asio/SimpleTcpClient.h                \
	../Include/Asio/SimpleTcpServer.h                \
	../Include/Asio/TcpClient.h                      \
	../Include/Asio/TcpConnection.h                  \
	../Include/Asio/TcpConnections.h                 \
	../Include/Asio/TcpServer.h                      \
	../Include/Asio/TcpTypedClient.h                 \
	../Include/Asio/TcpTypedServer.h                 \
	../Include/CsvGrid/CsvGridCell.h                 \
    ../Include/CsvGrid/CsvGridCellDouble.h           \
	../Include/CsvGrid/CsvGridRow.h                  \
	../Include/CsvGrid/CsvGridMain.h                 \
	../Include/CsvGrid/CsvGrid.h                     \
	../Include/DebugLog/DebugLog.h                   \
	../Include/DebugLog/DebugLogSingleton.h          \
	../Include/DebugLog/DebugLogging.h               \
	../Include/Exceptions/CustomException.h          \
    ../Include/FileUtils/FileUtils.h                 \
	../Include/IniFile/IniFileLines.h                \
	../Include/IniFile/IniFileSectionDetails.h       \
	../Include/IniFile/IniFile.h                     \
	../Include/Serialization/SerializationIncludes.h \
	../Include/Serialization/SerializeToVector.h     \
	../Include/Sorting/GenericSorting.h              \
	../Include/StringUtils/StringUtils.h             \
	../Include/Threads/BoundedBuffer.h               \
	../Include/Threads/ConcurrentQueue.h             \
	../Include/Threads/JoinThreads.h                 \
	../Include/Threads/MessageQueueThread.h          \
	../Include/Threads/SyncEvent.h                   \
	../Include/Threads/ThreadBase.h                  \
	../Include/Threads/ThreadGroup.h                 \
	../Include/Asio/UdpSender.h                      \
	../Include/Asio/UdpReceiver.h                    \
    ../Include/Asio/MulticastReceiver.h              \
    ../Include/Asio/MulticastSender.h                \
	../Include/Asio/UdpTypedSender.h                 \
    ../Include/Asio/SimpleUdpSender.h                \
    ../Include/Asio/SimpleUdpReceiver.h              \
    ../Include/Platform/PlatformDefines.h            \
    ../Include/CoreLibraryDllGlobal.h                \
    ../Include/Asio/MulticastTypedSender.h           \
    ../Include/Asio/SimpleMulticastSender.h          \
    ../Include/Asio/SimpleMulticastReceiver.h        \
    ../Include/Threads/ThreadRunner.h

SOURCES +=                                             \
    ../Source/Asio/AsioDefines.cpp                     \
	../Source/Asio/IoServiceThreadGroup.cpp            \
	../Source/Asio/MessageUtils.cpp                    \
	../Source/Asio/SimpleTcpClient.cpp                 \
	../Source/Asio/SimpleTcpServer.cpp                 \
	../Source/Asio/TcpClient.cpp                       \
	../Source/Asio/TcpConnection.cpp                   \
	../Source/Asio/TcpConnections.cpp                  \
	../Source/Asio/TcpServer.cpp                       \
	../Source/CsvGrid/CsvGridCell.cpp                  \
    ../Source/CsvGrid/CsvGridCellDouble.cpp           \
	../Source/CsvGrid/CsvGridRow.cpp                   \
	../Source/CsvGrid/CsvGridMain.cpp                  \
	../Source/DebugLog/DebugLog.cpp                    \
	../Source/Exceptions/CustomException.cpp           \
    ../Source/FileUtils/FileUtils.cpp                  \
	../Source/IniFile/IniFileLines.cpp                 \
	../Source/IniFile/IniFileSectionDetails.cpp        \
	../Source/IniFile/IniFile.cpp                      \
	../Source/Sorting/GenericSorting.cpp               \
	../Source/StringUtils/StringUtils.cpp              \
	../Source/Threads/ConcurrentQueue.cpp              \
	../Source/Threads/MessageQueueThread.cpp           \
	../Source/Threads/SyncEvent.cpp                    \
	../Source/Threads/ThreadBase.cpp                   \
	../Source/Threads/ThreadGroup.cpp                  \
	../Source/Asio/UdpSender.cpp                       \
	../Source/Asio/UdpReceiver.cpp                     \
    ../Source/Asio/MulticastReceiver.cpp               \
    ../Source/Asio/MulticastSender.cpp                 \
    ../Source/Asio/SimpleUdpSender.cpp                 \
    ../Source/Asio/SimpleUdpReceiver.cpp               \
    ../Source/Asio/SimpleMulticastSender.cpp           \
    ../Source/Asio/SimpleMulticastReceiver.cpp         \
    ../Source/Threads/ThreadRunner.cpp

DISTFILES += CoreLibraryDll_resource.rc
	
	
	
	
