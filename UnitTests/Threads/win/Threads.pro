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
CONFIG += boost

QMAKE_CXXFLAGS += -std=c++11

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

SOURCES += ../tst_ThreadsTest.cpp \
    ../../../Threads/ConcurrentQueue.cpp \
    ../../../Threads/MessageQueueThread.cpp \
    ../../../Threads/SyncEvent.cpp \
    ../../../Threads/ThreadBase.cpp \
    ../../../Threads/ThreadGroup.cpp \
    ../../../Exceptions/CustomException.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../ConcurrentQueue.hpp \
    ../../../Exceptions/CustomException.hpp \
    ../../../MessageQueueThread.hpp \
    ../../../SyncEvent.hpp \
    ../../../ThreadBase.hpp \
    ../../../ThreadGroup.hpp \
    ../../../BoundedBuffer.hpp \
    ../../../Threads/JoinThreads.hpp
