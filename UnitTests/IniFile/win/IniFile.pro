#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T19:48:20
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_IniFileTest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += core_lib

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

SOURCES += ../tst_IniFileTest.cpp \
    ../../../IniFile/IniFile.cpp \
    ../../../StringUtils/StringUtils.cpp \
    ../../../Exceptions/CustomException.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../../../StringUtils.hpp \
    ../../../IniFile.hpp \
    ../../../Exceptions/CustomException.hpp
