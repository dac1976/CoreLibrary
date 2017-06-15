TEMPLATE = app
CONFIG += console debug_and_release build_all
CONFIG -= app_bundle
CONFIG += qt
CONFIG += google_test
# Use these 2 when building against static lib.
#CONFIG += core_lib
#DEFINES += CORE_LIBRARY_LIB
# Use this when building against DLL.
CONFIG += core_lib_dll

# Use these to selectively disable certain unit tests.
# DEFINES += DISABLE_ASIO_TESTS
# DEFINES += DISABLE_CSVGRID_TESTS
# DEFINES += DISABLE_DEBUGLOG_TESTS
# DEFINES += DISABLE_SORTING_TESTS
# DEFINES += DISABLE_INIFILE_TESTS
# DEFINES += DISABLE_SERIALIZATION_TESTS
# DEFINES += DISABLE_STRINGUTILS_TESTS
# DEFINES += DISABLE_THREADS_TESTS
# DEFINES += DISABLE_FILEUTILS_TESTS

# On Windows we do this, assumes we'll be using MS VC 2015.
win32 {
    # disable incremental linking with debug builds
    QMAKE_LFLAGS_DEBUG += /INCREMENTAL:NO

    # Due to exporting from DLL we might get suprious warnings.
    QMAKE_CXXFLAGS += /wd4251 /wd4275 /wd4100
    DEFINES += _CRT_SECURE_NO_WARNINGS=1

    # Set binary's output folder.
    # This is for x86 builds.
    !contains(QMAKE_TARGET.arch, x86_64) {
        CONFIG(debug, debug|release) {
          DESTDIR = debug/x86
        } else {
          DESTDIR = release/x86
        }
    }
    # This is for x64 builds.
    else {
        CONFIG(debug, debug|release) {
          DESTDIR = debug/x64
        } else {
          DESTDIR = release/x64
        }
    }
}
# On non-windows, assumed to beLinux, we do ths.
else {
    # Make sure we enable C++14 support.
    QMAKE_CXXFLAGS += -std=c++14

    # Set binary's output folder.
    CONFIG(debug, debug|release) {
      DESTDIR = debug
    } else {
      DESTDIR = release
    }
}

OBJECTS_DIR = $${DESTDIR}/obj
MOC_DIR = $${DESTDIR}/moc
RCC_DIR = $${DESTDIR}/rcc
UI_DIR = $${DESTDIR}/ui

SOURCES += \
    ../tst_SerializationUtilsTest.cpp \
    ../tst_GenericSortingTest.cpp \
    ../tst_StringUtilsTest.cpp \
    ../tst_FileUtilsTest.cpp \
    ../tst_IniFileTest.cpp \
    ../tst_ThreadsTest.cpp \
    ../tst_DebugLogTest.cpp \
    ../tst_AsioTest.cpp \
    ../tst_CsvGridTest.cpp

HEADERS += \
    ../../../Include/CoreLibraryDllGlobal.h

include(deployment.pri)
qtcAddDeployment()
