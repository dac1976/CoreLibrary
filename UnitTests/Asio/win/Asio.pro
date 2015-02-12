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

HEADERS +=                                                       \
	../../../Include/Exceptions/CustomException.hpp          \
	../../../Include/Threads/SyncEvent.hpp                   \
	../../../Include/Threads/ThreadGroup.hpp                 \
	../../../Include/Threads/JoinThreads.hpp                 \
	../../../Include/Asio/IoServiceThreadGroup.hpp           \
	../../../Include/Asio/AsioDefines.hpp                    \
	../../../Include/Asio/MessageUtils.hpp                   \
	../../../Include/Asio/TcpConnection.hpp                  \
	../../../Include/Asio/TcpConnections.hpp                 \
	../../../Include/Asio/TcpServer.hpp                      \
	../../../Include/Asio/TcpClient.hpp                      \
	../../../Include/Serialization/SerializationIncludes.hpp \
	../../../Include/Serialization/SerializeToVector.hpp     \
	../../../Include/Asio/TcpTypedClient.hpp                 \
	../../../Include/Asio/TcpTypedServer.hpp                 \
	../../../Include/Asio/SimpleTcpClient.hpp                \
	../../../Include/Asio/SimpleTcpServer.hpp                \
	../../../Include/Asio/UdpSender.hpp                      \
	../../../Include/Asio/UdpReceiver.hpp                    \
	../../../Include/Asio/UdpTypedSender.hpp

SOURCES +=                                             \
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
	../../../Source/Asio/UdpReceiver.cpp
