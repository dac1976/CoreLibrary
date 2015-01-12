#-------------------------------------------------
#
# Project created by QtCreator 2013-08-14T19:48:20
#
#-------------------------------------------------

QT       += testlib
QT       -= gui

TARGET = tst_CsvGridTest
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
    ../../../Include/StringUtils/StringUtils.hpp    \
	../../../Include/CsvGrid/CsvGridCell.hpp        \ 
	../../../Include/CsvGrid/CsvGridRow.hpp         \
	../../../Include/CsvGrid/CsvGridMain.hpp        \
    ../../../Include/CsvGrid/CsvGrid.hpp            \
    ../../../Include/Exceptions/CustomException.hpp

SOURCES += ../tst_CsvGridTest.cpp                  \
    ../../../Source/CsvGrid/CsvGridCell.cpp        \
	../../../Source/CsvGrid/CsvGridRow.cpp         \
    ../../../Source/CsvGrid/CsvGridMain.cpp        \
    ../../../Source/StringUtils/StringUtils.cpp    \
    ../../../Source/Exceptions/CustomException.cpp
