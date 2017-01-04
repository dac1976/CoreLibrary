#ifndef DISABLE_FILEUTILS_TESTS

#include "FileUtils/FileUtils.h"
#include "boost/predef.h"
#include "boost/filesystem.hpp"

#include "gtest/gtest.h"

using namespace core_lib::file_utils;
namespace bfs = boost::filesystem;

#if BOOST_OS_LINUX
    static const bfs::path data_base_path     = "/home/duncan/Projects/CoreLibrary/UnitTests/GoogleTests/data";
    static const bfs::path alt_base_path      = "/home/duncan/Projects/CoreLibrary/UnitTests/GoogleTests";
    static const bfs::path copy_base_path     = "/home/duncan/Projects/CoreLibrary/UnitTests/GoogleTests/data_copy";
    static const bfs::path alt_copy_base_path = "/home/duncan/Projects/CoreLibrary/UnitTests/data_copy";
#else
    static const bfs::path data_base_path     = "../data";
    static const bfs::path alt_base_path      = "../";
    static const bfs::path copy_base_path     = "../data_copy";
    static const bfs::path alt_copy_base_path = "../../data_copy";
#endif

TEST(FileUtilsTest, Case1_CommonRoot_SameLevel)
{
    bfs::path path1 = data_base_path;
    path1 /= "testfile1.csv";
    path1 =bfs::system_complete(path1);

    bfs::path path2 = data_base_path;
    path2 /= "testfile2.csv";
    path2 =bfs::system_complete(path2);

    bfs::path rootComp = path2.parent_path();
    bfs::path rootPath = FindCommonRootPath(path1.string(), path2.string());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case2_CommonRoot_DiffLevels)
{
    bfs::path path1 = data_base_path;
    path1 /= "testfile1.csv";
    path1 =bfs::system_complete(path1);

    bfs::path path2 = alt_base_path;
    path2 /= "tst_FileUtilsTest.cpp";
    path2 =bfs::system_complete(path2);

    bfs::path rootComp = path2.parent_path();
    bfs::path rootPath = FindCommonRootPath(path1.string(), path2.string());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case3_CopyDirectory_SameLevel)
{
    bfs::path source = bfs::system_complete(data_base_path);
    bfs::path target = bfs::system_complete(copy_base_path);

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case4_CopyDirectory_TargetExists_Allowed)
{
    bfs::path source = bfs::system_complete(data_base_path);
    bfs::path target = bfs::system_complete(copy_base_path);

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case5_CopyDirectory_TargetExists_Disallowed)
{
    bfs::path source = bfs::system_complete(data_base_path);
    bfs::path target = bfs::system_complete(copy_base_path);

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_THROW(CopyDirectoryRecursively(source.string(), target.string(), eCopyDirectoryOptions::failIfTargetExists), xCopyDirectoryError);
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case6_CopyDirectory_DiffLevels)
{    
    bfs::path source = bfs::system_complete(data_base_path);
    bfs::path target = bfs::system_complete(alt_copy_base_path);

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case7_ListDirectoryEntries_1)
{    
    bfs::path dir = bfs::system_complete(data_base_path);

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string()));

    EXPECT_EQ(files.size(), 9U);
}

TEST(FileUtilsTest, Case8_ListDirectoryEntries_2)
{
    bfs::path dir = bfs::system_complete(data_base_path);

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".csv"));

    EXPECT_EQ(files.size(), 2U);
}

TEST(FileUtilsTest, Case9_ListDirectoryEntries_3)
{
    bfs::path dir = bfs::system_complete(data_base_path);

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".ini"));

    EXPECT_EQ(files.size(), 7U);
}

TEST(FileUtilsTest, Case10_ListDirectoryEntries_4)
{
    bfs::path dir = data_base_path;
    dir /= "test_file_1.ini";
    dir =bfs::system_complete(dir);

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".ini"));

    EXPECT_EQ(files.size(), 0U);
}


#endif // DISABLE_FILEUTILS_TESTS
