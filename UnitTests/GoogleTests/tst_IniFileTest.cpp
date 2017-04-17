#ifndef DISABLE_INIFILE_TESTS

#include <fstream>
#include <sstream>
#include "boost/predef.h"
#define BOOST_NO_CXX11_SCOPED_ENUMS
#include "boost/filesystem.hpp"
#include "IniFile/IniFile.h"

#include "gtest/gtest.h"

#if BOOST_OS_LINUX
static const std::string path1 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_1.ini";
static const std::string path2 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_2.ini";
static const std::string path3 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_3.ini";
static const std::string path4 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_4.ini";
static const std::string path5 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_5.ini";
static const std::string path6 =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_6.ini";
static const std::string path_check =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_check.ini";
static const std::string path_invalid =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file.ini";
static const std::string path_temp =
    "/home/duncan/projects/corelibrary/UnitTests/GoogleTests/data/test_file_tmp.ini";
#else
static const std::string path1        = "../../../data/test_file_1.ini";
static const std::string path2        = "../../../data/test_file_2.ini";
static const std::string path3        = "../../../data/test_file_3.ini";
static const std::string path4        = "../../../data/test_file_4.ini";
static const std::string path5        = "../../../data/test_file_5.ini";
static const std::string path6        = "../../../data/test_file_6.ini";
static const std::string path_check   = "../../../data/test_file_check.ini";
static const std::string path_invalid = "../../../data/test_file.ini";
static const std::string path_temp    = "../../../data/test_file_tmp.ini";
#endif

TEST(IniFileTest, Case1_InvalidLine)
{
    bool correctException;

    try
    {
        core_lib::ini_file::IniFile iniFile(path1);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("file contains invalid line") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path2);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("file contains invalid key") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path3);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("file contains invalid section") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path4);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("file contains duplicate key") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path5);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("file contains duplicate section") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path_invalid);
        correctException = false;
    }
    catch (core_lib::ini_file::xIniFileParserError& e)
    {
        if (std::string(e.what()).compare("cannot create ifstream") == 0)
        {
            correctException = true;
        }
        else
        {
            correctException = false;
        }
    }
    catch (...)
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
        core_lib::ini_file::IniFile iniFile(path6);
        noException = true;
    }
    catch (...)
    {
        noException = false;
    }

    EXPECT_TRUE(noException);
}

TEST(IniFileTest, Case8_ValidFileCompare)
{
    boost::filesystem::copy_file(
        path6, path_temp, boost::filesystem::copy_option::overwrite_if_exists);
    bool noException;

    try
    {
        core_lib::ini_file::IniFile iniFile;
        iniFile.LoadFile(path_temp);
        iniFile.WriteInt32("Section 2", "key5", static_cast<int>(1));
        iniFile.UpdateFile();
        noException = true;
    }
    catch (...)
    {
        noException = false;
    }

    EXPECT_TRUE(noException);

    std::ifstream iniFileA(path_temp);
    std::ifstream iniFileB(path_check);

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

    while (iniFileA.good() && iniFileB.good())
    {
        std::string lineA, lineB;
        std::getline(iniFileA, lineA);
        std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
    }

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
    iniFileA.close();
    iniFileB.close();

    boost::filesystem::remove(path_temp);
}

TEST(IniFileTest, Case9_CopyConstructor)
{
    core_lib::ini_file::IniFile iniFile1(path_check);
    core_lib::ini_file::IniFile iniFile2(iniFile1);
    iniFile2.UpdateFile(path_temp);

    std::ifstream iniFileA(path_temp);
    std::ifstream iniFileB(path_check);

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

    while (iniFileA.good() && iniFileB.good())
    {
        std::string lineA, lineB;
        std::getline(iniFileA, lineA);
        std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
    }

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
    iniFileA.close();
    iniFileB.close();

    boost::filesystem::remove(path_temp);
}

TEST(IniFileTest, Case10_MoveConstructor)
{
    core_lib::ini_file::IniFile iniFile(core_lib::ini_file::IniFile{path_check});
    iniFile.UpdateFile(path_temp);

    std::ifstream iniFileA(path_temp);
    std::ifstream iniFileB(path_check);

    EXPECT_TRUE(iniFileA.is_open() && iniFileA.good());
    EXPECT_TRUE(iniFileB.is_open() && iniFileB.good());

    while (iniFileA.good() && iniFileB.good())
    {
        std::string lineA, lineB;
        std::getline(iniFileA, lineA);
        std::getline(iniFileB, lineB);

        EXPECT_STREQ(lineA.c_str(), lineB.c_str());
    }

    EXPECT_TRUE(iniFileA.eof() && iniFileB.eof());
    iniFileA.close();
    iniFileB.close();

    boost::filesystem::remove(path_temp);
}

TEST(IniFileTest, Case11_GetSections)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    std::list<std::string>      sections(iniFile.GetSections());

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
    core_lib::ini_file::IniFile   iniFile(path_check);
    core_lib::ini_file::keys_list keys(iniFile.GetSection("Section 1"));

    EXPECT_EQ(keys.size(), 5U);
}

TEST(IniFileTest, Case13_SectionExists)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    EXPECT_TRUE(iniFile.SectionExists("Section 1"));
    EXPECT_TRUE(iniFile.SectionExists("Section 2"));
    EXPECT_FALSE(iniFile.SectionExists("I Don't Exist"));
}

TEST(IniFileTest, Case14_KeyExists)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    EXPECT_TRUE(iniFile.KeyExists("Section 1", "key1"));
    EXPECT_TRUE(iniFile.KeyExists("Section 2", "key3"));
    EXPECT_FALSE(iniFile.KeyExists("I Don't Exist", "Nor Do I"));
}

TEST(IniFileTest, Case15_ReadValues)
{
    core_lib::ini_file::IniFile   iniFile(path_check);
    core_lib::ini_file::keys_list keys(iniFile.GetSection("Section 1"));

    EXPECT_EQ(keys.size(), 5U);

    int keyCnt = 1;

    for (const auto& key : keys)
    {
        switch (keyCnt++)
        {
        case 1:
        {
            auto value = iniFile.ReadString("Section 1", key.first);
            EXPECT_STREQ(value.c_str(), "value as a string");
        }
        break;
        case 2:
        {
            auto value = iniFile.ReadInt32("Section 1", key.first);
            EXPECT_EQ(value, 123456);
        }
        break;
        case 3:
        {
            auto value = iniFile.ReadDouble("Section 1", key.first);
            EXPECT_EQ(value, 123.456789);
        }
        break;
        case 4:
        {
            auto value = iniFile.ReadString("Section 1", key.first);
            EXPECT_EQ(value, "string with number 1234");
        }
        break;
        case 5:
        {
            auto value = iniFile.ReadInt32("Section 1", key.first);
            EXPECT_EQ(value, 1);
        }
        break;
        }
    }
}

TEST(IniFileTest, Case16_EraseSection)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    EXPECT_TRUE(iniFile.SectionExists("Section 1"));
    iniFile.EraseSection("Section 1");
    EXPECT_FALSE(iniFile.SectionExists("Section 1"));
}

TEST(IniFileTest, Case17_EraseKey)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    EXPECT_TRUE(iniFile.KeyExists("Section 1", "key1"));
    iniFile.EraseKey("Section 1", "key1");
    EXPECT_FALSE(iniFile.KeyExists("Section 1", "key1"));
}

TEST(IniFileTest, Case18_EraseKeys)
{
    core_lib::ini_file::IniFile   iniFile(path_check);
    core_lib::ini_file::keys_list keys(iniFile.GetSection("Section 1"));
    EXPECT_EQ(keys.size(), 5U);
    iniFile.EraseKeys("Section 1");
    keys = iniFile.GetSection("Section 1");
    EXPECT_EQ(keys.size(), 0U);
}

TEST(IniFileTest, Case19_AddKeyAtEndOfLastSection)
{
    core_lib::ini_file::IniFile iniFile(path_check);
    iniFile.WriteString("Section 2", "Test Key", "Test Value");
    std::string value = iniFile.ReadString("Section 2", "Test Key", "");
    EXPECT_STREQ(value.c_str(), "Test Value");
}

#endif // DISABLE_INIFILE_TESTS
