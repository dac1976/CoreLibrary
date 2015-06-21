#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T12:33:11
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_DebugLogTest
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

HEADERS +=                                          \
    ../../../Include/DebugLog/DebugLog.h          \
    ../../../Include/Threads/ConcurrentQueue.h    \
    ../../../Include/Exceptions/CustomException.h \
    ../../../Include/Threads/MessageQueueThread.h \
    ../../../Include/Threads/SyncEvent.h          \
    ../../../Include/Threads/ThreadBase.h         \
    ../../../Include/DebugLog/DebugLogging.h      \
    ../../../Include/DebugLog/DebugLogSingleton.h \
	../../../Include/Platform/PlatformDefines.h

SOURCES += ../tst_DebugLogTest.cpp                 \
    ../../../Source/DebugLog/DebugLog.cpp          \
    ../../../Source/Threads/ConcurrentQueue.cpp    \
    ../../../Source/Threads/MessageQueueThread.cpp \
    ../../../Source/Threads/SyncEvent.cpp          \
    ../../../Source/Threads/ThreadBase.cpp         \
    ../../../Source/Exceptions/CustomException.cpp
