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

SOURCES += ../tst_GenericSortingTest.cpp \
    ../../../Exceptions/CustomException.cpp \
    ../../../Sorting/GenericSorting.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../GenericSorting.hpp \
    ../../../CustomException.hpp
