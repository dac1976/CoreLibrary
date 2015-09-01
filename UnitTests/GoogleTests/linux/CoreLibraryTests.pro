TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += core_lib
CONFIG += google_test

# Need to add in pthread as google test requires it.
QMAKE_CXXFLAGS += -std=c++14

SOURCES += \
	../tst_SerializationUtilsTest.cpp \
    ../tst_GenericSortingTest.cpp \
    ../tst_StringUtilsTest.cpp \
    ../tst_IniFileTest.cpp \
    ../tst_ThreadsTest.cpp

include(deployment.pri)
qtcAddDeployment()

