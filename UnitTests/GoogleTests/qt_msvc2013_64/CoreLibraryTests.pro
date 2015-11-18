TEMPLATE = app
CONFIG += console debug_and_release build_all
CONFIG -= app_bundle
CONFIG += qt
CONFIG += google_test_64


# Use these 2 when building against static lib.
#CONFIG += core_lib_64
#DEFINES += CORE_LIBRARY_LIB
# Use this when building against DLL.
CONFIG += core_lib_64_dll

# Use these to selectively disable certain unit tests.
# DEFINES += DISABLE_ASIO_TESTS
# DEFINES += DISABLE_CSVGRID_TESTS
# DEFINES += DISABLE_DEBUGLOG_TESTS
# DEFINES += DISABLE_SORTING_TESTSDEFINES += DISABLE_INIFILE_TESTS
# DEFINES += DISABLE_SERIALIZATION_TESTS
# DEFINES += DISABLE_STRINGUTILS_TESTS
# DEFINES += DISABLE_THREADS_TESTS

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
