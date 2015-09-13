TEMPLATE = app
CONFIG += console debug_and_release build_all
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += core_lib_64
CONFIG += google_test_64

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
