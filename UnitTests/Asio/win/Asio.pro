#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T12:33:11
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_AsioTest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += core_lib

QMAKE_CXXFLAGS += -std=c++14

TEMPLATE = app

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

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS +=                                                   \
	../../../Include/Exceptions/CustomException.h          \
	../../../Include/Threads/SyncEvent.h                   \
	../../../Include/Threads/ThreadGroup.h                 \
	../../../Include/Threads/JoinThreads.h                 \
	../../../Include/Asio/IoServiceThreadGroup.h           \
	../../../Include/Asio/AsioDefines.h                    \
	../../../Include/Asio/MessageUtils.h                   \
	../../../Include/Asio/TcpConnection.h                  \
	../../../Include/Asio/TcpConnections.h                 \
	../../../Include/Asio/TcpServer.h                      \
	../../../Include/Asio/TcpClient.h                      \
	../../../Include/Serialization/SerializationIncludes.h \
	../../../Include/Serialization/SerializeToVector.h     \
	../../../Include/Asio/TcpTypedClient.h                 \
	../../../Include/Asio/TcpTypedServer.h                 \
	../../../Include/Asio/SimpleTcpClient.h                \
	../../../Include/Asio/SimpleTcpServer.h                \
	../../../Include/Asio/UdpSender.h                      \
	../../../Include/Asio/UdpReceiver.h                    \
	../../../Include/Asio/UdpTypedSender.h                 \
    ../../../Include/Asio/SimpleUdpSender.h                \
    ../../../Include/Asio/SimpleUdpReceiver.h

SOURCES +=                                         \
	../tst_AsioTest.cpp                            \
	../../../Source/Exceptions/CustomException.cpp \
	../../../Source/Threads/SyncEvent.cpp          \
	../../../Source/Threads/ThreadGroup.cpp        \
	../../../Source/Asio/IoServiceThreadGroup.cpp  \
	../../../Source/Asio/MessageUtils.cpp          \
	../../../Source/Asio/TcpConnection.cpp         \
	../../../Source/Asio/TcpConnections.cpp        \
	../../../Source/Asio/TcpServer.cpp             \
	../../../Source/Asio/TcpClient.cpp             \
	../../../Source/Asio/SimpleTcpServer.cpp       \
	../../../Source/Asio/SimpleTcpClient.cpp       \
	../../../Source/Asio/UdpSender.cpp             \
	../../../Source/Asio/UdpReceiver.cpp           \
    ../../../Source/Asio/SimpleUdpSender.cpp       \
    ../../../Source/Asio/SimpleUdpReceiver.cpp
