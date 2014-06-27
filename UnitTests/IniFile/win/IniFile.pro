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
CONFIG += boost

QMAKE_CXXFLAGS += -std=c++11

TEMPLATE = app

CONFIG(debug, debug|release) {
  # TARGET = $$join(TARGET,,,d) # if compiling in debug mode, append a "d" to the application name
  DESTDIR = debug
  LIBS += C:/Projects/ThirdParty/boost_1_55_0/lib/gcc_482_x86/libboost_filesystem-mgw48-d-1_55.a \
          C:/Projects/ThirdParty/boost_1_55_0/lib/gcc_482_x86/libboost_system-mgw48-d-1_55.a
} else {
  DESTDIR = release
  LIBS += C:/Projects/ThirdParty/boost_1_55_0/lib/gcc_482_x86/libboost_filesystem-mgw48-1_55.a \
          C:/Projects/ThirdParty/boost_1_55_0/lib/gcc_482_x86/libboost_system-mgw48-1_55.a
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
