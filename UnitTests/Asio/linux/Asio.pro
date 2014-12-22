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

HEADERS +=                                      \
	../../../Exceptions/CustomException.hpp \
	../../../SyncEvent.hpp                  \
	../../../ThreadGroup.hpp                \
	../../../Threads/JoinThreads.hpp        \
	../../../Asio/IoServiceThreadGroup.hpp  \
	../../../Asio/AsioDefines.hpp           \
	../../../Asio/MessageUtils.hpp          \
	../../../Asio/TcpConnection.hpp         \
	../../../Asio/TcpConnections.hpp        \
	../../../Asio/TcpServer.hpp             \
	../../../Asio/TcpClient.hpp             \
	../../../Serialization/SerializationIncludes.hpp \
	../../../Serialization/SerializeToVector.hpp     \
	../../../TcpTypedClient.hpp                      \
	../../../TcpTypedServer.hpp

SOURCES +=                                      \
	../tst_AsioTest.cpp                     \
	../../../Exceptions/CustomException.cpp \
	../../../Threads/SyncEvent.cpp          \
	../../../Threads/ThreadGroup.cpp        \
	../../../Asio/IoServiceThreadGroup.cpp  \
	../../../Asio/MessageUtils.cpp          \
	../../../Asio/TcpConnection.cpp         \
	../../../Asio/TcpConnections.cpp        \
	../../../Asio/TcpServer.cpp             \
	../../../Asio/TcpClient.cpp             \
	../../../Asio/TcpTypedClient.cpp        \
	../../../Asio/TcpTypedServer.cpp
