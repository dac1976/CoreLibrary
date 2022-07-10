#-------------------------------------------------
#
# Project created by QtCreator 2013-07-20T22:13:38
#
#-------------------------------------------------

QT       -= gui

CONFIG(debug, debug|release) {
  TARGET = CoreLibraryDlld
} else {
  TARGET = CoreLibraryDll
}

TEMPLATE = lib
CONFIG += dll debug_and_release build_all
CONFIG += boost \
          boost_libs \
          loki  \
          cereal
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
    win32-msvc{
        # disable incremental linking with debug builds
        QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

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

        DISTFILES += CoreLibraryDll_resource.rc
    } else{
        # Make sure we enable C++17 support.
        QMAKE_CXXFLAGS += -std=c++17

        # Set version info for library.
        VERSION = 1.7.1

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
}
# On non-windows, assumed to be Linux, we do this.
else {
    # Make sure we enable C++17 support.
    QMAKE_CXXFLAGS += -std=c++17

    # Set version info for library.
    VERSION = 1.7.1

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

INCLUDEPATH += \
    $$(CORE_LIBRARY) \
    $$(CORE_LIBRARY)/Include

HEADERS += \
    $$(CORE_LIBRARY)/Include/Asio/AsioDefines.h                    \
    $$(CORE_LIBRARY)/Include/Asio/IoContextThreadGroup.h           \
    $$(CORE_LIBRARY)/Include/Asio/MemoryUtils.h                    \
    $$(CORE_LIBRARY)/Include/Asio/MessageUtils.h                   \
    $$(CORE_LIBRARY)/Include/Asio/SimpleTcpClient.h                \
    $$(CORE_LIBRARY)/Include/Asio/SimpleTcpClientList.h            \
    $$(CORE_LIBRARY)/Include/Asio/SimpleTcpServer.h                \
    $$(CORE_LIBRARY)/Include/Asio/TcpClient.h                      \
    $$(CORE_LIBRARY)/Include/Asio/TcpClientList.h                  \
    $$(CORE_LIBRARY)/Include/Asio/TcpConnection.h                  \
    $$(CORE_LIBRARY)/Include/Asio/TcpConnections.h                 \
    $$(CORE_LIBRARY)/Include/Asio/TcpServer.h                      \
    $$(CORE_LIBRARY)/Include/Asio/TcpTypedClient.h                 \
    $$(CORE_LIBRARY)/Include/Asio/TcpTypedServer.h                 \
    $$(CORE_LIBRARY)/Include/CsvGrid/CsvGridCell.h                 \
    $$(CORE_LIBRARY)/Include/CsvGrid/CsvGridCellDouble.h           \
    $$(CORE_LIBRARY)/Include/CsvGrid/CsvGridRow.h                  \
    $$(CORE_LIBRARY)/Include/CsvGrid/CsvGridMain.h                 \
    $$(CORE_LIBRARY)/Include/CsvGrid/CsvGrid.h                     \
    $$(CORE_LIBRARY)/Include/DebugLog/DebugLog.h                   \
    $$(CORE_LIBRARY)/Include/DebugLog/DebugLogSingleton.h          \
    $$(CORE_LIBRARY)/Include/DebugLog/DebugLogging.h               \
    $$(CORE_LIBRARY)/Include/FileUtils/FileUtils.h                 \
    $$(CORE_LIBRARY)/Include/IniFile/IniFileLines.h                \
    $$(CORE_LIBRARY)/Include/IniFile/IniFileSectionDetails.h       \
    $$(CORE_LIBRARY)/Include/IniFile/IniFile.h                     \
    $$(CORE_LIBRARY)/Include/Serialization/SerializationIncludes.h \
    $$(CORE_LIBRARY)/Include/Serialization/SerializeToVector.h     \
    $$(CORE_LIBRARY)/Include/Sorting/GenericSorting.h              \
    $$(CORE_LIBRARY)/Include/StringUtils/StringUtils.h             \
    $$(CORE_LIBRARY)/Include/Threads/BoundedBuffer.h               \
    $$(CORE_LIBRARY)/Include/Threads/ConcurrentQueue.h             \
    $$(CORE_LIBRARY)/Include/Threads/JoinThreads.h                 \
    $$(CORE_LIBRARY)/Include/Threads/MessageQueueThread.h          \
    $$(CORE_LIBRARY)/Include/Threads/SyncEvent.h                   \
    $$(CORE_LIBRARY)/Include/Threads/ThreadBase.h                  \
    $$(CORE_LIBRARY)/Include/Threads/ThreadGroup.h                 \
    $$(CORE_LIBRARY)/Include/Threads/ThreadPriority.h              \
    $$(CORE_LIBRARY)/Include/Threads/ThreadGroup.h                 \
    $$(CORE_LIBRARY)/Include/Threads/DeadlineTimer.h               \
    $$(CORE_LIBRARY)/Include/Threads/EventThread.h                 \
    $$(CORE_LIBRARY)/Include/Asio/UdpSender.h                      \
    $$(CORE_LIBRARY)/Include/Asio/UdpReceiver.h                    \
    $$(CORE_LIBRARY)/Include/Asio/MulticastReceiver.h              \
    $$(CORE_LIBRARY)/Include/Asio/MulticastSender.h                \
    $$(CORE_LIBRARY)/Include/Asio/UdpTypedSender.h                 \
    $$(CORE_LIBRARY)/Include/Asio/SimpleUdpSender.h                \
    $$(CORE_LIBRARY)/Include/Asio/SimpleUdpReceiver.h              \
    $$(CORE_LIBRARY)/Include/Platform/PlatformDefines.h            \
    $$(CORE_LIBRARY)/Include/CoreLibraryDllGlobal.h                \
    $$(CORE_LIBRARY)/Include/Asio/MulticastTypedSender.h           \
    $$(CORE_LIBRARY)/Include/Asio/SimpleMulticastSender.h          \
    $$(CORE_LIBRARY)/Include/Asio/SimpleMulticastReceiver.h        \
    $$(CORE_LIBRARY)/Include/Threads/ThreadRunner.h                \
    $$(CORE_LIBRARY)/Include/Exceptions/DetailedException.h

SOURCES += \
    $$(CORE_LIBRARY)/Source/Asio/AsioDefines.cpp                     \
    $$(CORE_LIBRARY)/Source/Asio/IoContextThreadGroup.cpp            \
    $$(CORE_LIBRARY)/Source/Asio/MessageUtils.cpp                    \
    $$(CORE_LIBRARY)/Source/Asio/SimpleTcpClient.cpp                 \
    $$(CORE_LIBRARY)/Source/Asio/SimpleTcpClientList.cpp             \
    $$(CORE_LIBRARY)/Source/Asio/SimpleTcpServer.cpp                 \
    $$(CORE_LIBRARY)/Source/Asio/TcpClient.cpp                       \
    $$(CORE_LIBRARY)/Source/Asio/TcpClientList.cpp                   \
    $$(CORE_LIBRARY)/Source/Asio/TcpConnection.cpp                   \
    $$(CORE_LIBRARY)/Source/Asio/TcpConnections.cpp                  \
    $$(CORE_LIBRARY)/Source/Asio/TcpServer.cpp                       \
    $$(CORE_LIBRARY)/Source/CsvGrid/CsvGridCell.cpp                  \
    $$(CORE_LIBRARY)/Source/CsvGrid/CsvGridCellDouble.cpp            \
    $$(CORE_LIBRARY)/Source/DebugLog/DebugLog.cpp                    \
    $$(CORE_LIBRARY)/Source/FileUtils/FileUtils.cpp                  \
    $$(CORE_LIBRARY)/Source/IniFile/IniFileLines.cpp                 \
    $$(CORE_LIBRARY)/Source/IniFile/IniFileSectionDetails.cpp        \
    $$(CORE_LIBRARY)/Source/IniFile/IniFile.cpp                      \
    $$(CORE_LIBRARY)/Source/StringUtils/StringUtils.cpp              \
    $$(CORE_LIBRARY)/Source/Threads/SyncEvent.cpp                    \
    $$(CORE_LIBRARY)/Source/Threads/ThreadBase.cpp                   \
    $$(CORE_LIBRARY)/Source/Threads/ThreadGroup.cpp                  \
    $$(CORE_LIBRARY)/Source/Threads/ThreadPriority.cpp               \
    $$(CORE_LIBRARY)/Source/Threads/ThreadRunner.cpp                 \
    $$(CORE_LIBRARY)/Source/Threads/EventThread.cpp                  \
    $$(CORE_LIBRARY)/Source/Threads/DeadlineTimer.cpp                \
    $$(CORE_LIBRARY)/Source/Asio/UdpSender.cpp                       \
    $$(CORE_LIBRARY)/Source/Asio/UdpReceiver.cpp                     \
    $$(CORE_LIBRARY)/Source/Asio/MulticastReceiver.cpp               \
    $$(CORE_LIBRARY)/Source/Asio/MulticastSender.cpp                 \
    $$(CORE_LIBRARY)/Source/Asio/SimpleUdpSender.cpp                 \
    $$(CORE_LIBRARY)/Source/Asio/SimpleUdpReceiver.cpp               \
    $$(CORE_LIBRARY)/Source/Asio/SimpleMulticastSender.cpp           \
    $$(CORE_LIBRARY)/Source/Asio/SimpleMulticastReceiver.cpp




