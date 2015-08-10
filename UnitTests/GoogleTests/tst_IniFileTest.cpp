#include <fstream>
#include "boost/predef.h"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"
#include "IniFile/IniFile.h"

#include "gtest/gtest.h"

TEST(IniFileTest, Case1_InvalidLine)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_1.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_1.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case2_InvalidKey)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_2.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_2.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case3_InvalidSection)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_3.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_3.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case4_DuplicateKey)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_4.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_4.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case5_DuplicateSection)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_5.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_5.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case6_InvalidFile)
{
	bool correctException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file.ini");
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

    EXPECT_TRUE(correctException);
}

TEST(IniFileTest, Case7_ValidFile)
{
	bool noException;

	try
	{
#if BOOST_OS_WINDOWS
        core_lib::ini_file::IniFile iniFile("../data/test_file_6.ini");
#else
        core_lib::ini_file::IniFile iniFile("../../data/test_file_6.ini");
#endif
		noException = true;
	}
	catch(...)
	{
		noException = false;
	}

    EXPECT_TRUE(noException);
}

TEST(IniFileTest, Case8_ValidFileCompare)
{
#if BOOST_OS_WINDOWS
    boost::filesystem::copy_file("../data/test_file_6.ini", "../data/test_file_tmp.ini"
								 , boost::filesystem::copy_option::overwrite_if_exists);
#else
    boost::filesystem::copy_file("../../data/test_file_6.ini", "../../data/test_file_tmp.ini"
								 , boost::filesystem::copy_option::overwrite_if_exists);
#endif

	bool noException;

	try
	{
		core_lib::ini_file::IniFile iniFile;
#if BOOST_OS_WINDOWS
        iniFile.LoadFile("../data/test_file_tmp.ini");
#else
        iniFile.LoadFile("../../data/test_file_tmp.ini");
#endif
		iniFile.WriteValue("Section 2", "key5", static_cast<int>(1));
		iniFile.UpdateFile();
		noException = true;
	}
	catch(...)
	{
		noException = false;
	}

    EXPECT_TRUE(noException);

#if BOOST_OS_WINDOWS
    std::ifstream iniFileA("../data/test_file_tmp.ini");
    std::ifstream iniFileB("../data/test_file_check.ini");
#else
    std::ifstream iniFileA("../../data/test_file_tmp.ini");
    std::ifstream iniFileB("../../data/test_file_check.ini");
#endif

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
	}

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
    boost::filesystem::remove("../data/test_file_tmp.ini");
#else
    boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case9_CopyConstructor)
{
#if BOOST_OS_WINDOWS
    core_lib::ini_file::IniFile iniFile1("../data/test_file_check.ini");
#else
    core_lib::ini_file::IniFile iniFile1("../../data/test_file_check.ini");
#endif

	core_lib::ini_file::IniFile iniFile2(iniFile1);

#if BOOST_OS_WINDOWS
    iniFile2.UpdateFile("../data/test_file_tmp.ini");
#else
    iniFile2.UpdateFile("../../data/test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS
    std::ifstream iniFileA("../data/test_file_tmp.ini");
    std::ifstream iniFileB("../data/test_file_check.ini");
#else
    std::ifstream iniFileA("../../data/test_file_tmp.ini");
    std::ifstream iniFileB("../../data/test_file_check.ini");
#endif

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
	}

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
    boost::filesystem::remove("../data/test_file_tmp.ini");
#else
    boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case10_MoveConstructor)
{
#if BOOST_OS_WINDOWS
    core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../data/test_file_check.ini"));
#else
    core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../../data/test_file_check.ini"));
#endif

#if BOOST_OS_WINDOWS
    iniFile.UpdateFile("../data/test_file_tmp.ini");
#else
    iniFile.UpdateFile("../../data/test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS
    std::ifstream iniFileA("../data/test_file_tmp.ini");
    std::ifstream iniFileB("../data/test_file_check.ini");
#else
    std::ifstream iniFileA("../../data/test_file_tmp.ini");
    std::ifstream iniFileB("../../data/test_file_check.ini");
#endif

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

	while(iniFileA.good() && iniFileB.good())
	{
		std::string lineA, lineB;
		std::getline(iniFileA, lineA);
		std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
	}

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
	iniFileA.close();
	iniFileB.close();

#if BOOST_OS_WINDOWS
    boost::filesystem::remove("../data/test_file_tmp.ini");
#else
    boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case11_GetSections)
{
#if BOOST_OS_WINDOWS
    core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
#else
    core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

	std::list<std::string> sections{iniFile.GetSections()};

    EXPECT_EQ(sections.size(), 2U);

	//TODO
}

TEST(IniFileTest, Case12_GetSection)
{
	//TODO
}
