TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += core_lib
CONFIG += google_test

QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
    ../tst_SerializationUtilsTest.cpp \
    ../tst_GenericSortingTest.cpp \
    ../tst_StringUtilsTest.cpp \
    ../tst_IniFileTest.cpp \
    ../tst_ThreadsTest.cpp \
    ../tst_DebugLogTest.cpp \
    ../tst_AsioTest.cpp \
    ../tst_CsvGridTest.cpp

include(deployment.pri)
qtcAddDeployment()
