#-------------------------------------------------
#
# Project created by QtCreator 2013-08-06T21:41:32
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_StringUtilsTest
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

SOURCES += ../tst_StringUtilsTest.cpp             \
    ../../../Source/StringUtils/StringUtils.cpp   \
    ../../../Source/Exceptions/CustomException.cpp \
	../../../Include/Platform/PlatformDefines.h

HEADERS +=                                          \
    ../../../Include/StringUtils/StringUtils.h    \
    ../../../Include/Exceptions/CustomException.h
