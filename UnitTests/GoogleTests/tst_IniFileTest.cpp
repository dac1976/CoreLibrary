#include <fstream>
#include <sstream>
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
	#ifdef _MSC_VER
        core_lib::ini_file::IniFile iniFile("../../data/test_file_1.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_1.ini");
	#endif
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
	#ifdef _MSC_VER
        core_lib::ini_file::IniFile iniFile("../../data/test_file_2.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_2.ini");
	#endif
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
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_3.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_3.ini");
	#endif
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
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_4.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_4.ini");
	#endif
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
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_5.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_5.ini");
	#endif
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
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file.ini");
	#endif
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
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_6.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_6.ini");
	#endif
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
    #ifdef _MSC_VER
		boost::filesystem::copy_file("../../data/test_file_6.ini", "../../data/test_file_tmp.ini"
			, boost::filesystem::copy_option::overwrite_if_exists);
	#else
		boost::filesystem::copy_file("../data/test_file_6.ini", "../data/test_file_tmp.ini"
			, boost::filesystem::copy_option::overwrite_if_exists);
	#endif
#else
    boost::filesystem::copy_file("../../data/test_file_6.ini", "../../data/test_file_tmp.ini"
								 , boost::filesystem::copy_option::overwrite_if_exists);
#endif

	bool noException;

	try
	{
		core_lib::ini_file::IniFile iniFile;
#if BOOST_OS_WINDOWS
	#ifdef _MSC_VER
		iniFile.LoadFile("../../data/test_file_tmp.ini");
	#else
		iniFile.LoadFile("../data/test_file_tmp.ini");
	#endif
#else
        iniFile.LoadFile("../../data/test_file_tmp.ini");
#endif
        iniFile.WriteInt32("Section 2", "key5", static_cast<int>(1));
		iniFile.UpdateFile();
		noException = true;
	}
	catch(...)
	{
		noException = false;
	}

    EXPECT_TRUE(noException);

#if BOOST_OS_WINDOWS
	#ifdef _MSC_VER
		std::ifstream iniFileA("../../data/test_file_tmp.ini");
		std::ifstream iniFileB("../../data/test_file_check.ini");
	#else
		std::ifstream iniFileA("../data/test_file_tmp.ini");
		std::ifstream iniFileB("../data/test_file_check.ini");
	#endif
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
	#ifdef _MSC_VER
		boost::filesystem::remove("../../data/test_file_tmp.ini");
	#else
		boost::filesystem::remove("../data/test_file_tmp.ini");
	#endif
#else
    boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case9_CopyConstructor)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile1("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile1("../data/test_file_check.ini");
	#endif
#else
    core_lib::ini_file::IniFile iniFile1("../../data/test_file_check.ini");
#endif

	core_lib::ini_file::IniFile iniFile2(iniFile1);

#if BOOST_OS_WINDOWS   
	#ifdef _MSC_VER
		iniFile2.UpdateFile("../../data/test_file_tmp.ini");
	#else
		iniFile2.UpdateFile("../data/test_file_tmp.ini");
	#endif
#else
    iniFile2.UpdateFile("../../data/test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		std::ifstream iniFileA("../../data/test_file_tmp.ini");
		std::ifstream iniFileB("../../data/test_file_check.ini");
	#else
		std::ifstream iniFileA("../data/test_file_tmp.ini");
		std::ifstream iniFileB("../data/test_file_check.ini");
	#endif
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
	#ifdef _MSC_VER
		boost::filesystem::remove("../../data/test_file_tmp.ini");
	#else
		boost::filesystem::remove("../data/test_file_tmp.ini");
	#endif
#else
    boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case10_MoveConstructor)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../../data/test_file_check.ini"));
	#else
		core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../data/test_file_check.ini"));
	#endif
#else
	core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile("../../data/test_file_check.ini"));
#endif

#if BOOST_OS_WINDOWS   
	#ifdef _MSC_VER
		iniFile.UpdateFile("../../data/test_file_tmp.ini");
	#else
		iniFile.UpdateFile("../data/test_file_tmp.ini");
	#endif
#else
	iniFile.UpdateFile("../../data/test_file_tmp.ini");
#endif

#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		std::ifstream iniFileA("../../data/test_file_tmp.ini");
		std::ifstream iniFileB("../../data/test_file_check.ini");
	#else
		std::ifstream iniFileA("../data/test_file_tmp.ini");
		std::ifstream iniFileB("../data/test_file_check.ini");
	#endif
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
	#ifdef _MSC_VER
		boost::filesystem::remove("../../data/test_file_tmp.ini");
	#else
		boost::filesystem::remove("../data/test_file_tmp.ini");
	#endif
#else
	boost::filesystem::remove("../../data/test_file_tmp.ini");
#endif
}

TEST(IniFileTest, Case11_GetSections)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
	#endif
#else
	core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

    std::list<std::string> sections(iniFile.GetSections());

    EXPECT_EQ(sections.size(), 2U);

    int secCnt = 1;

    for (const auto& section : sections)
    {
        std::stringstream ss;
        ss << "Section " << secCnt++;
        EXPECT_STREQ(ss.str().c_str(), section.c_str());
    }
}

TEST(IniFileTest, Case12_GetSection)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
	#endif
#else
	core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

    core_lib::ini_file::keys_list keys(iniFile.GetSection("Section 1"));

    EXPECT_EQ(keys.size(), 5U);
}

TEST(IniFileTest, Case13_SectionExists)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
	#endif
#else
	core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

    EXPECT_TRUE(iniFile.SectionExists("Section 1"));
    EXPECT_TRUE(iniFile.SectionExists("Section 2"));
    EXPECT_FALSE(iniFile.SectionExists("I Don't Exist"));
}

TEST(IniFileTest, Case14_KeyExists)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
	#endif
#else
	core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

    EXPECT_TRUE(iniFile.KeyExists("Section 1", "key1"));
    EXPECT_TRUE(iniFile.KeyExists("Section 2", "key3"));
    EXPECT_FALSE(iniFile.KeyExists("I Don't Exist", "Nor Do I"));
}

TEST(IniFileTest, Case15_ReadValues)
{
#if BOOST_OS_WINDOWS    
	#ifdef _MSC_VER
		core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
	#else
		core_lib::ini_file::IniFile iniFile("../data/test_file_check.ini");
	#endif
#else
	core_lib::ini_file::IniFile iniFile("../../data/test_file_check.ini");
#endif

    core_lib::ini_file::keys_list keys(iniFile.GetSection("Section 1"));

    EXPECT_EQ(keys.size(), 5U);

    int keyCnt = 1;

    for (const auto& key : keys)
    {
        switch (keyCnt++)
        {
        case 1:
            {
                auto value =  iniFile.ReadString("Section 1", key.first);
                EXPECT_STREQ(value.c_str(), "value as a string");
            }
            break;
        case 2:
            {
                auto value =  iniFile.ReadInt32("Section 1", key.first);
                EXPECT_EQ(value, 123456);
            }
            break;
        case 3:
            {
                auto value =  iniFile.ReadDouble("Section 1", key.first);
                EXPECT_EQ(value, 123.456789);
            }
            break;
        case 4:
            {
                auto value =  iniFile.ReadString("Section 1", key.first);
                EXPECT_EQ(value, "string with number 1234");
            }
            break;
        case 5:
            {
                auto value =  iniFile.ReadInt32("Section 1", key.first);
                EXPECT_EQ(value, 1);
            }
            break;
        }
    }
}
