#-------------------------------------------------
#
# Project created by QtCreator 2013-12-30T12:33:11
#
#-------------------------------------------------
DEFINES += LOKI_CLASS_LEVEL_THREADING

QT       += testlib
QT       -= gui

TARGET = tst_DebugLogTest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += boost
CONFIG += loki

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

CONFIG(debug, debug|release) {
  # TARGET = $$join(TARGET,,,d) # if compiling in debug mode, append a "d" to the application name
  DESTDIR = debug
  LIBS += /home/duncan/projects/boost_1_55_0/lib/libboost_filesystem.a \
          /home/duncan/projects/boost_1_55_0/lib/libboost_system.a
} else {
  DESTDIR = release
  LIBS += /home/duncan/projects/boost_1_55_0/lib/libboost_filesystem.a \
          /home/duncan/projects/boost_1_55_0/lib/libboost_system.a
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

SOURCES += ../tst_DebugLogTest.cpp \
    ../../../DebugLog/DebugLog.cpp \
    ../../../Threads/ConcurrentQueue.cpp \
    ../../../Threads/MessageQueueThread.cpp \
    ../../../Threads/SyncEvent.cpp \
    ../../../Threads/ThreadBase.cpp \
    ../../../Exceptions/CustomException.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../DebugLog/DebugLog.hpp \
    ../../../ConcurrentQueue.hpp \
    ../../../Exceptions/CustomException.hpp \
    ../../../MessageQueueThread.hpp \
    ../../../SyncEvent.hpp \
    ../../../ThreadBase.hpp \
    ../../../DebugLogging.hpp
