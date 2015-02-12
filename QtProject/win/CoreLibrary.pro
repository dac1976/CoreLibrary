#-------------------------------------------------
#
# Project created by QtCreator 2013-07-20T22:13:38
#
#-------------------------------------------------

QT       -= gui

TARGET = CoreLibrary
TEMPLATE = lib
CONFIG += staticlib debug_and_release build_all
CONFIG += core_lib_settings

QMAKE_CXXFLAGS += -std=c++14

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
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

HEADERS +=                                                    \
	../../Include/Asio/IoServiceThreadGroup.hpp           \
	../../Include/Asio/AsioDefines.hpp                    \
	../../Include/Asio/MessageUtils.hpp                   \
	../../Include/Asio/SimpleTcpClient.hpp                \
	../../Include/Asio/SimpleTcpServer.hpp                \
	../../Include/Asio/TcpClient.hpp                      \
	../../Include/Asio/TcpConnection.hpp                  \
	../../Include/Asio/TcpConnections.hpp                 \
	../../Include/Asio/TcpServer.hpp                      \
	../../Include/Asio/TcpTypedClient.hpp                 \
	../../Include/Asio/TcpTypedServer.hpp                 \
	../../Include/CsvGrid/CsvGridCell.hpp                 \
	../../Include/CsvGrid/CsvGridRow.hpp                  \
	../../Include/CsvGrid/CsvGridMain.hpp                 \
	../../Include/CsvGrid/CsvGrid.hpp                     \
	../../Include/DebugLog/DebugLog.hpp                   \
	../../Include/DebugLog/DebugLogSingleton.hpp          \
	../../Include/DebugLog/DebugLogging.hpp               \
	../../Include/Exceptions/CustomException.hpp          \
	../../Include/IniFile/IniFileLines.hpp                \
	../../Include/IniFile/IniFileSectionDetails.hpp       \
	../../Include/IniFile/IniFile.hpp                     \
	../../Include/Serialization/SerializationIncludes.hpp \
	../../Include/Serialization/SerializeToVector.hpp     \
	../../Include/Sorting/GenericSorting.hpp              \
	../../Include/StringUtils/StringUtils.hpp             \
	../../Include/Threads/BoundedBuffer.hpp               \
	../../Include/Threads/ConcurrentQueue.hpp             \
	../../Include/Threads/JoinThreads.hpp                 \
	../../Include/Threads/MessageQueueThread.hpp          \
	../../Include/Threads/SyncEvent.hpp                   \
	../../Include/Threads/ThreadBase.hpp                  \
	../../Include/Threads/ThreadGroup.hpp                 \
	../../Include/Asio/UdpSender.hpp                      \
	../../Include/Asio/UdpReceiver.hpp                    \
	../../Include/Asio/UdpTypedSender.hpp

SOURCES +=                                                    \
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
	../../Source/Asio/UdpReceiver.cpp
	
	
	
	
