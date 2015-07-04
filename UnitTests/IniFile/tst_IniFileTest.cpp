#include <QString>
#include <QtTest>
#include <fstream>
#include "boost/predef.h"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"
#include "IniFile/IniFile.h"

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
	void Case7_ValidFile();
	void Case8_ValidFileCompare();
	void Case9_CopyConstructor();
	void Case10_MoveConstructor();
	void Case11_GetSections();
	void Case12_GetSection();
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
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("file contains invalid line") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
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
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("file contains invalid key") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
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
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("file contains invalid section") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
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
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("file contains duplicate key") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
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
		core_lib::ini_file::IniFile iniFile("../test_file_5.ini");
#else
		core_lib::ini_file::IniFile iniFile("../../test_file_5.ini");
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("file contains duplicate section") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
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
#endif
		correctException = false;
	}
	catch(core_lib::ini_file::xIniFileParserError& e)
	{
		if (e.whatStr().compare("cannot create ifstream") == 0)
		{
			correctException = true;
		}
		else
		{
			correctException = false;
		}
	}
	catch(...)
	{
		correctException = false;
	}

	QVERIFY(correctException);
}

void IniFileTest::Case7_ValidFile()
{
	bool noException;

	try
	{
#if BOOST_OS_WINDOWS
		core_lib::ini_file::IniFile iniFile("../test_file_6.ini");
#else
		core_lib::ini_file::IniFile iniFile("../../test_file_6.ini");
#endif
		noException = true;
	}
	catch(...)
	{
		noException = false;
	}

	QVERIFY(noException);
}

void IniFileTest::Case8_ValidFileCompare()
{
#if BOOST_OS_WINDOWS
	boost::filesystem::copy_file("../test_file_6.ini", "../test_file_tmp.ini"
								 , boost::filesystem::copy_option::overwrite_if_exists);
#else
	boost::filesystem::copy_file("../../test_file_6.ini", "../../test_file_tmp.ini"
								 , boost::filesystem::copy_option::overwrite_if_exists);
#endif

	bool noException;

	try
	{
		core_lib::ini_file::IniFile iniFile;
#if BOOST_OS_WINDOWS
		iniFile.LoadFile("../test_file_tmp.ini");
#else
		iniFile.LoadFile("../../test_file_tmp.ini");
#endif
		iniFile.WriteValue("Section 2", "key5", static_cast<int>(1));
		iniFile.UpdateFile();
		noException = true;
	}
	catch(...)
	{
		noException = false;
	}

	QVERIFY(noException);

#if BOOST_OS_WINDOWS
	std::ifstream iniFileA("../test_file_tmp.ini");
	std::ifstream iniFileB("../test_file_check.ini");
#else
	std::ifstream iniFileA("../../test_file_tmp.ini");
	std::ifstream iniFileB("../../test_file_check.ini");
#endif

	QVERIFY(iniFileA.is_open() && iniFileA.good());
	QVERIFY(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

		QCOMPARE(lineA, lineB);
	}

	QVERIFY(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
	boost::filesystem::remove("../test_file_tmp.ini");
#else
	boost::filesystem::remove("../../test_file_tmp.ini");
#endif
}

void IniFileTest::Case9_CopyConstructor()
{
#if BOOST_OS_WINDOWS
	core_lib::ini_file::IniFile iniFile1("../test_file_check.ini");
#else
	core_lib::ini_file::IniFile iniFile1("../../test_file_check.ini");
#endif

	core_lib::ini_file::IniFile iniFile2(iniFile1);

#if BOOST_OS_WINDOWS
	iniFile2.UpdateFile("../test_file_tmp.ini");
#else
	iniFile2.UpdateFile("../../test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS
	std::ifstream iniFileA("../test_file_tmp.ini");
	std::ifstream iniFileB("../test_file_check.ini");
#else
	std::ifstream iniFileA("../../test_file_tmp.ini");
	std::ifstream iniFileB("../../test_file_check.ini");
#endif

	QVERIFY(iniFileA.is_open() && iniFileA.good());
	QVERIFY(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

		QCOMPARE(lineA, lineB);
	}

	QVERIFY(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
	boost::filesystem::remove("../test_file_tmp.ini");
#else
	boost::filesystem::remove("../../test_file_tmp.ini");
#endif
}

void IniFileTest::Case10_MoveConstructor()
{
#if BOOST_OS_WINDOWS
	core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../test_file_check.ini"));
#else
	core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../../test_file_check.ini"));
#endif

#if BOOST_OS_WINDOWS
	iniFile.UpdateFile("../test_file_tmp.ini");
#else
	iniFile.UpdateFile("../../test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS
	std::ifstream iniFileA("../test_file_tmp.ini");
	std::ifstream iniFileB("../test_file_check.ini");
#else
	std::ifstream iniFileA("../../test_file_tmp.ini");
	std::ifstream iniFileB("../../test_file_check.ini");
#endif

	QVERIFY(iniFileA.is_open() && iniFileA.good());
	QVERIFY(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

		QCOMPARE(lineA, lineB);
	}

	QVERIFY(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
	boost::filesystem::remove("../test_file_tmp.ini");
#else
	boost::filesystem::remove("../../test_file_tmp.ini");
#endif
}

void IniFileTest::Case11_GetSections()
{
#if BOOST_OS_WINDOWS
	core_lib::ini_file::IniFile iniFile("../test_file_check.ini");
#else
	core_lib::ini_file::IniFile iniFile("../../test_file_check.ini");
#endif

	std::list<std::string> sections{iniFile.GetSections()};

	QVERIFY(sections.size() == 2);

	//TODO
}

void IniFileTest::Case12_GetSection()
{
	//TODO
}

QTEST_APPLESS_MAIN(IniFileTest)

#include "tst_IniFileTest.moc"
