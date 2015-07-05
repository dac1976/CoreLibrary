TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += core_lib
CONFIG += google_test

# Need to add in pthread as google test requires it.
QMAKE_CXXFLAGS += -std=c++14

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    ../../../../ThirdParty/google_test/include/gtest/gtest.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest_pred_impl.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest_prod.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-death-test.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-message.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-param-test.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-printers.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-spi.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-test-part.h \
    ../../../../ThirdParty/google_test/include/gtest/gtest-typed-test.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-death-test-internal.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-filepath.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-internal.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-linked_ptr.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-param-util.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-param-util-generated.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-port.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-string.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-tuple.h \
    ../../../../ThirdParty/google_test/include/gtest/internal/gtest-type-util.h \
    ../../../../ThirdParty/google_test/src/gtest-internal-inl.h

SOURCES += \
    ../tst_SerializationUtilsTest.cpp \
    ../tst_GenericSortingTest.cpp \
    ../../../../ThirdParty/google_test/src/gtest.cc \
    ../../../../ThirdParty/google_test/src/gtest_main.cc \
    ../../../../ThirdParty/google_test/src/gtest-all.cc \
    ../../../../ThirdParty/google_test/src/gtest-death-test.cc \
    ../../../../ThirdParty/google_test/src/gtest-filepath.cc \
    ../../../../ThirdParty/google_test/src/gtest-port.cc \
    ../../../../ThirdParty/google_test/src/gtest-printers.cc \
    ../../../../ThirdParty/google_test/src/gtest-test-part.cc \
    ../../../../ThirdParty/google_test/src/gtest-typed-test.cc
