TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += core_lib

# Need to add in pthread as google test requires it.
QMAKE_CXXFLAGS += -std=c++14 -pthread

INCLUDEPATH += /home/duncan/Projects/ThirdParty/google_test/include

# The order of these libs is important and libpthread.so must go
# after the libgtest_main.a's linkage.
LIBS += -L/home/duncan/Projects/ThirdParty/google_test/make -lgtest_main \
		-L/lib64 -lpthread

SOURCES += \
	../tst_SerializationUtilsTest.cpp \
    ../tst_GenericSortingTest.cpp

include(deployment.pri)
qtcAddDeployment()

