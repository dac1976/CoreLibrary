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

HEADERS +=                               \
	../../StringUtils.hpp                \
	../../CsvGrid/CsvGridCell.hpp        \
	../../CsvGrid/CsvGridRow.hpp         \
	../../CsvGrid/CsvGridMain.hpp        \
	../../CsvGrid.hpp                    \
	../../Exceptions/CustomException.hpp \
	../../GenericSorting.hpp             \
	../../SyncEvent.hpp                  \
	../../ConcurrentQueue.hpp            \
	../../ThreadBase.hpp                 \
	../../ThreadGroup.hpp                \
	../../MessageQueueThread.hpp         \
	../../BoundedBuffer.hpp              \
	../../DebugLog/DebugLog.hpp          \
	../../DebugLogging.hpp               \
	../../IniFile.hpp                    \
	../../Threads/JoinThreads.hpp        \
	../../Asio/IoServiceThreadGroup.hpp  \
	../../DebugLog/DebugLogSingleton.hpp \
	../../IniFile/IniFileLines.hpp       \
	../../IniFile/IniFileSectionDetails.hpp \
	../../Serialization/SerializeToVector.hpp \
	../../Asio/AsioDefines.hpp \
	../../Asio/TcpConnection.hpp \
	../../Asio/TcpConnections.hpp \
	../../Asio/TcpServer.hpp \
	../../Serialization/SerializationIncludes.hpp

SOURCES +=                               \
	../../StringUtils/StringUtils.cpp    \
	../../CsvGrid/CsvGridCell.cpp        \
	../../CsvGrid/CsvGridRow.cpp         \
	../../CsvGrid/CsvGridMain.cpp        \
	../../Exceptions/CustomException.cpp \
	../../Sorting/GenericSorting.cpp     \
	../../Threads/SyncEvent.cpp          \
	../../Threads/ConcurrentQueue.cpp    \
	../../Threads/ThreadBase.cpp         \
	../../Threads/ThreadGroup.cpp        \
	../../Threads/MessageQueueThread.cpp \
	../../DebugLog/DebugLog.cpp          \
	../../IniFile/IniFile.cpp            \
	../../Asio/IoServiceThreadGroup.cpp  \
	../../IniFile/IniFileLines.cpp       \
	../../IniFile/IniFileSectionDetails.cpp \
	../../Asio/TcpConnection.cpp \
	../../Asio/TcpConnections.cpp \
	../../Asio/TcpServer.cpp
