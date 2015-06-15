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

HEADERS +=                                             \
    ../../../Include/StringUtils/StringUtils.h       \
    ../../../Include/IniFile/IniFile.h               \
    ../../../Include/Exceptions/CustomException.h    \
    ../../../Include/IniFile/IniFileLines.h          \
    ../../../Include/IniFile/IniFileSectionDetails.h

SOURCES += ../tst_IniFileTest.cpp                     \
    ../../../Source/IniFile/IniFile.cpp               \
    ../../../Source/StringUtils/StringUtils.cpp       \
    ../../../Source/Exceptions/CustomException.cpp    \
    ../../../Source/IniFile/IniFileLines.cpp          \
    ../../../Source/IniFile/IniFileSectionDetails.cpp