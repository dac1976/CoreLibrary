#include <QString>
#include <QtTest>
#include "../../IniFile.hpp"
#include <fstream>
#include "boost/predef.h"

class IniFileTest : public QObject
{
    Q_OBJECT

public:
    IniFileTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();
    void Case1_InvalidLine();
    void Case2_InvalidKey();
    void Case3_InvalidSection();
    void Case4_DuplicateKey();
    void Case5_DuplicateSection();
    void Case6_InvalidFile();
};

IniFileTest::IniFileTest()
{
}

void IniFileTest::initTestCase()
{
}

void IniFileTest::cleanupTestCase()
{
}

void IniFileTest::Case1_InvalidLine()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file_1.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file_1.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("file contains invalid line") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void IniFileTest::Case2_InvalidKey()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file_2.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file_2.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("file contains invalid key") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void IniFileTest::Case3_InvalidSection()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file_3.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file_3.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("file contains invalid section") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void IniFileTest::Case4_DuplicateKey()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file_4.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file_4.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("file contains duplicate key") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void IniFileTest::Case5_DuplicateSection()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file_4.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file_5.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("file contains duplicate section") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

void IniFileTest::Case6_InvalidFile()
{
    bool correctException;

    try
    {
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../test_file.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../test_file.ini");
        correctException = false;
#endif
    }
    catch(core_lib::ini_file::xIniFileParserError& e)
    {
        if (e.whatStr().compare("cannot create ifstream") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = true;
        }
    }
    catch(...)
    {
        correctException = false;
    }

    QVERIFY(correctException);
}

QTEST_APPLESS_MAIN(IniFileTest)

#include "tst_IniFileTest.moc"
