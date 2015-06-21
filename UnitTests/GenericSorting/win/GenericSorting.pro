#-------------------------------------------------
#
# Project created by QtCreator 2013-10-03T15:20:06
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_GenericSortingTest
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
    ../../../Include/Sorting/GenericSorting.h     \
    ../../../Include/Exceptions/CustomException.h \
    ../../../Include/Platform/PlatformDefines.h

SOURCES += ../tst_GenericSortingTest.cpp           \
    ../../../Source/Exceptions/CustomException.cpp \
    ../../../Source/Sorting/GenericSorting.cpp
