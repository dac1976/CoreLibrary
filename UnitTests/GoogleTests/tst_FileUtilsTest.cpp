#ifndef DISABLE_FILEUTILS_TESTS

#include "FileUtils/FileUtils.h"
#include "boost/predef.h"
#include "boost/filesystem.hpp"

#include "gtest/gtest.h"

using namespace core_lib::file_utils;
namespace bfs = boost::filesystem;

TEST(FileUtilsTest, Case1_CommonRoot_SameLevel)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path path1 = bfs::system_complete("../../data/testfile1.csv");
        const bfs::path path2 = bfs::system_complete("../../data/testfile2.csv");
    #else
        const bfs::path path1 = bfs::system_complete("../data/testfile1.csv");
        const bfs::path path2 = bfs::system_complete("../data/testfile2.csv");
    #endif
#else
    const bfs::path path1 = bfs::system_complete("../../data/testfile1.csv");
    const bfs::path path2 = bfs::system_complete("../../data/testfile2.csv");
#endif

    const bfs::path rootComp = path2.parent_path();
    const bfs::path rootPath = FindCommonRootPath(path1.string(), path2.string());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case2_CommonRoot_DiffLevels)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path path1 = bfs::system_complete("../../data/testfile1.csv");
        const bfs::path path2 = bfs::system_complete("../../tst_FileUtilsTest.cpp");
    #else
        const bfs::path path1 = bfs::system_complete("../data/testfile1.csv");
        const bfs::path path2 = bfs::system_complete("../tst_FileUtilsTest.cpp");
    #endif
#else
    const bfs::path path1 = bfs::system_complete("../../data/testfile1.csv");
    const bfs::path path2 = bfs::system_complete("../../tst_FileUtilsTest.cpp");
#endif

    const bfs::path rootComp = path2.parent_path();
    const bfs::path rootPath = FindCommonRootPath(path1.string(), path2.string());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case3_CopyDirectory_SameLevel)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path source = bfs::system_complete("../../data");
        const bfs::path target = bfs::system_complete("../../data_copy");
    #else
    const bfs::path source = bfs::system_complete("../data");
    const bfs::path target = bfs::system_complete("../data_copy");
    #endif
#else
    const bfs::path source = bfs::system_complete("../../data");
    const bfs::path target = bfs::system_complete("../../data_copy");
#endif

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case4_CopyDirectory_TargetExists_Allowed)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path source = bfs::system_complete("../../data");
        const bfs::path target = bfs::system_complete("../../data_copy");
    #else
    const bfs::path source = bfs::system_complete("../data");
    const bfs::path target = bfs::system_complete("../data_copy");
    #endif
#else
    const bfs::path source = bfs::system_complete("../../data");
    const bfs::path target = bfs::system_complete("../../data_copy");
#endif

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
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path source = bfs::system_complete("../../data");
        const bfs::path target = bfs::system_complete("../../data_copy");
    #else
    const bfs::path source = bfs::system_complete("../data");
    const bfs::path target = bfs::system_complete("../data_copy");
    #endif
#else
    const bfs::path source = bfs::system_complete("../../data");
    const bfs::path target = bfs::system_complete("../../data_copy");
#endif

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
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path source = bfs::system_complete("../../data");
        const bfs::path target = bfs::system_complete("../../../data_copy");
    #else
    const bfs::path source = bfs::system_complete("../data");
    const bfs::path target = bfs::system_complete("../../data_copy");
    #endif
#else
    const bfs::path source = bfs::system_complete("../../data");
    const bfs::path target = bfs::system_complete("../../../data_copy");
#endif

    EXPECT_FALSE(bfs::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.string(), target.string()));
    EXPECT_TRUE(bfs::exists(target));
    EXPECT_TRUE(bfs::remove_all(target) > 0);
    EXPECT_FALSE(bfs::exists(target));
}

TEST(FileUtilsTest, Case7_ListDirectoryEntries_1)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path dir = bfs::system_complete("../../data");
    #else
    const bfs::path dir = bfs::system_complete("../data");
    #endif
#else
    const bfs::path dir = bfs::system_complete("../../data");
#endif

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string()));

    EXPECT_EQ(files.size(), 9U);
}

TEST(FileUtilsTest, Case8_ListDirectoryEntries_2)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path dir = bfs::system_complete("../../data");
    #else
    const bfs::path dir = bfs::system_complete("../data");
    #endif
#else
    const bfs::path dir = bfs::system_complete("../../data");
#endif

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".csv"));

    EXPECT_EQ(files.size(), 2U);
}

TEST(FileUtilsTest, Case9_ListDirectoryEntries_3)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path dir = bfs::system_complete("../../data");
    #else
    const bfs::path dir = bfs::system_complete("../data");
    #endif
#else
    const bfs::path dir = bfs::system_complete("../../data");
#endif

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".ini"));

    EXPECT_EQ(files.size(), 7U);
}

TEST(FileUtilsTest, Case10_ListDirectoryEntries_4)
{
#if BOOST_OS_WINDOWS
    #if defined(BOOST_COMP_MSVC) & (_MSC_VER >= 1900)
        const bfs::path dir = bfs::system_complete("../../data/test_file_1.ini");
    #else
    const bfs::path dir = bfs::system_complete("../data/test_file_1.ini");
    #endif
#else
    const bfs::path dir = bfs::system_complete("../../data/test_file_1.ini");
#endif

    std::list<std::string> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.string(), ".ini"));

    EXPECT_EQ(files.size(), 0U);
}


#endif // DISABLE_FILEUTILS_TESTS
