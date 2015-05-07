#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T12:33:11
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_ThreadsTest
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
    ../../../Include/Threads/ConcurrentQueue.hpp    \
    ../../../Exceptions/CustomException.hpp         \
    ../../../Include/Threads/MessageQueueThread.hpp \
    ../../../Include/Threads/SyncEvent.hpp          \
    ../../../Include/Threads/ThreadBase.hpp         \
    ../../../Include/Threads/ThreadGroup.hpp        \
    ../../../Include/Threads/BoundedBuffer.hpp      \
    ../../../Include/Threads/JoinThreads.hpp \
    ../../../Include/Threads/ConcurrentQueue2.hpp

SOURCES += ../tst_ThreadsTest.cpp                  \
    ../../../Source/Threads/ConcurrentQueue.cpp    \
    ../../../Source/Threads/MessageQueueThread.cpp \
    ../../../Source/Threads/SyncEvent.cpp          \
    ../../../Source/Threads/ThreadBase.cpp         \
    ../../../Source/Threads/ThreadGroup.cpp        \
    ../../../Source/Exceptions/CustomException.cpp
