#ifndef DISABLE_FILEUTILS_TESTS

#include "FileUtils/FileUtils.h"
#include <boost/predef.h>
#include "FileUtils/SelectFileSystemLibrary.hpp" 
#include "gtest/gtest.h"

using namespace core_lib::file_utils;

const filesys::path data_base_path = L"./data";
const filesys::path alt_base_path  = L"./";
const filesys::path copy_base_path = L"./data_copy";
const filesys::path alt_copy_base_path = L"./alt_data_copy";
const filesys::path move_base_path = L"./data_move";
const filesys::path alt_move_base_path = L"C:/Projects/Common/Utils/Platform/FileSystem/alt_data_move";
const filesys::path subdirs_path      = L"./";

TEST(FileUtilsTest, Case1_CommonRoot_SameLevel)
{
    filesys::path path1 = data_base_path;
    path1 /= L"testfile1.csv";
    path1 = filesys::system_complete(path1);

    filesys::path path2 = data_base_path;
    path2 /= L"testfile2.csv";
    path2 = filesys::system_complete(path2);

    filesys::path rootComp = path2.parent_path();
    filesys::path rootPath = FindCommonRootPath(path1.wstring(), path2.wstring());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case2_CommonRoot_DiffLevels)
{
    filesys::path path1 = data_base_path;
    path1 /= L"testfile1.csv";
    path1 = filesys::system_complete(path1);

    filesys::path path2 = alt_base_path;
    path2 /= L"tst_FileUtilsTest.cpp";
    path2 = filesys::system_complete(path2);

    filesys::path rootComp = path2.parent_path();
    filesys::path rootPath = FindCommonRootPath(path1.wstring(), path2.wstring());

    EXPECT_EQ(rootPath, rootComp);
}

TEST(FileUtilsTest, Case3_CopyDirectory_SameLevel)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case4_CopyDirectory_TargetExists_Allowed)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case5_CopyDirectory_TargetExists_Disallowed)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_THROW(CopyDirectoryRecursively(
                     source.wstring(), target.wstring(), eCopyDirectoryOptions::failIfTargetExists),
                 std::runtime_error);
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case6_CopyDirectory_DiffLevels)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(alt_copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));
    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case7_ListDirectoryEntries_1)
{
    filesys::path dir = filesys::system_complete(data_base_path);

    std::list<std::wstring> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.wstring()));

    EXPECT_EQ(files.size(), 3U);
}

TEST(FileUtilsTest, Case8_ListDirectoryEntries_2)
{
    filesys::path dir = filesys::system_complete(data_base_path);

    std::list<std::wstring> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.wstring(), L".csv"));

    EXPECT_EQ(files.size(), 2U);
}

TEST(FileUtilsTest, Case9_ListDirectoryEntries_3)
{
    filesys::path dir = filesys::system_complete(data_base_path);

    std::list<std::wstring> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.wstring(), L".ini"));

    EXPECT_EQ(files.size(), 1U);
}

TEST(FileUtilsTest, Case10_ListDirectoryEntries_4)
{
    filesys::path dir = data_base_path;
    dir /= L"test_file_1.ini";
    dir = filesys::system_complete(dir);

    std::list<std::wstring> files;
    EXPECT_NO_THROW(files = ListDirectoryContents(dir.wstring(), L".ini"));

    EXPECT_EQ(files.size(), 0U);
}

TEST(FileUtilsTest, Case11_FindFileRecursively_1)
{
    std::wstring pathFound;

    EXPECT_TRUE(FindFileRecursively(
        filesys::system_complete(data_base_path).wstring(), L"test_file_1.ini", pathFound));

    auto finalPath = data_base_path;
    finalPath /= L"test_file_1.ini";

    EXPECT_TRUE(filesys::system_complete(finalPath).wstring() == pathFound);
}

TEST(FileUtilsTest, Case12_FindFileRecursively_2)
{
    std::wstring pathFound;

    EXPECT_TRUE(FindFileRecursively(
        filesys::system_complete(data_base_path).wstring(), L"test_file_1.ini", pathFound, false));

    EXPECT_TRUE(filesys::system_complete(data_base_path).wstring() == pathFound);
}

TEST(FileUtilsTest, Case13_FindFileRecursively_3)
{
    std::wstring pathFound;

    EXPECT_ANY_THROW(FindFileRecursively(
        filesys::system_complete(copy_base_path).wstring(), L"test_file_1.ini", pathFound));
}

TEST(FileUtilsTest, Case14_FindFileRecursively_4)
{
    std::wstring pathFound;

    EXPECT_FALSE(FindFileRecursively(
        filesys::system_complete(data_base_path).wstring(), L"test_file_666.ini", pathFound));
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_1)
{
    std::string cifsUrl = "\\\\host-pc\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_TRUE(user.empty());
    EXPECT_TRUE(password.empty());
    EXPECT_EQ(hostOrIp, "host-pc");
    EXPECT_EQ(truncatedUrl, cifsUrl);
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_2)
{
    std::string cifsUrl = "\\\\192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_TRUE(user.empty());
    EXPECT_TRUE(password.empty());
    EXPECT_EQ(hostOrIp, "192.168.1.1");
    EXPECT_EQ(truncatedUrl, cifsUrl);
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_3)
{
    std::string cifsUrl = "fred:abc123@\\\\host-pc\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_EQ(user, "fred");
    EXPECT_EQ(password, "abc123");
    EXPECT_EQ(hostOrIp, "host-pc");
    EXPECT_EQ(truncatedUrl, "\\\\host-pc\\share");
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_4)
{
    std::string cifsUrl = "fred:abc123@\\\\192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_EQ(user, "fred");
    EXPECT_EQ(password, "abc123");
    EXPECT_EQ(hostOrIp, "192.168.1.1");
    EXPECT_EQ(truncatedUrl, "\\\\192.168.1.1\\share");
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_5)
{
    std::string cifsUrl = "fred:abc123@192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_FALSE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_6)
{
    std::string cifsUrl = "fredabc123@\\\\192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_FALSE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_7)
{
    std::string cifsUrl = "fred:abc123\\\\192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_FALSE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_8)
{
    std::string cifsUrl = "fredabc123\\\\192.168.1.1\\share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_FALSE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_9)
{
    std::string cifsUrl = "//host-pc/share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_TRUE(user.empty());
    EXPECT_TRUE(password.empty());
    EXPECT_EQ(hostOrIp, "host-pc");
    EXPECT_EQ(truncatedUrl, cifsUrl);
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_10)
{
    std::string cifsUrl = "//192.168.1.1/share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_TRUE(user.empty());
    EXPECT_TRUE(password.empty());
    EXPECT_EQ(hostOrIp, "192.168.1.1");
    EXPECT_EQ(truncatedUrl, cifsUrl);
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_11)
{
    std::string cifsUrl = "fred:abc123@//host-pc/share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_EQ(user, "fred");
    EXPECT_EQ(password, "abc123");
    EXPECT_EQ(hostOrIp, "host-pc");
    EXPECT_EQ(truncatedUrl, "//host-pc/share");
}

TEST(FileUtilsTest, Case15_CIFSUrlValidator_12)
{
    std::string cifsUrl = "fred:abc123@//192.168.1.1/share";
    std::string user;
    std::string password;
    std::string hostOrIp;
    std::string truncatedUrl;

    ASSERT_TRUE(hgl::CheckCifsPath(cifsUrl, user, password, hostOrIp, truncatedUrl));

    EXPECT_EQ(user, "fred");
    EXPECT_EQ(password, "abc123");
    EXPECT_EQ(hostOrIp, "192.168.1.1");
    EXPECT_EQ(truncatedUrl, "//192.168.1.1/share");
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_1)
{
    std::string cifsUrl = "//host-pc/share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_2)
{
    std::string cifsUrl = "//192.168.1.1/share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_3)
{
    std::string cifsUrl = "fred:abc123@//host-pc/share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_4)
{
    std::string cifsUrl = "fred:abc123@//192.168.1.1/share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_5)
{
    std::string cifsUrl = "\\\\host-pc\\share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_6)
{
    std::string cifsUrl = "\\\\192.168.1.1\\share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_7)
{
    std::string cifsUrl = "fred:abc123@\\\\host-pc\\share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_IsValidCifsPath_8)
{
    std::string cifsUrl = "fred:abc123@\\\\192.168.1.1\\share";

    ASSERT_TRUE(hgl::IsValidCifsPath(cifsUrl));
}

TEST(FileUtilsTest, Case16_MoveDirectory_SameLevel)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(move_base_path);
    EXPECT_NO_THROW(MoveDirectoryAndContents(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case18_MoveDirectory_TargetExists_Allowed)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(move_base_path);
    EXPECT_NO_THROW(MoveDirectoryAndContents(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = filesys::system_complete(data_base_path);
    target = filesys::system_complete(copy_base_path);
    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(move_base_path);
    EXPECT_NO_THROW(MoveDirectoryAndContents(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case19_MoveDirectory_TargetExists_Disallowed)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(move_base_path);
    EXPECT_NO_THROW(MoveDirectoryAndContents(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = filesys::system_complete(data_base_path);
    target = filesys::system_complete(copy_base_path);
    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(move_base_path);
    EXPECT_THROW(MoveDirectoryAndContents(
                     source.wstring(), target.wstring(), eCopyDirectoryOptions::failIfTargetExists),
                 std::runtime_error);
    EXPECT_TRUE(filesys::exists(target));

    EXPECT_TRUE(filesys::remove_all(source) > 0);
    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

TEST(FileUtilsTest, Case20_MoveDirectory_DiffLevels)
{
    filesys::path source = filesys::system_complete(data_base_path);
    filesys::path target = filesys::system_complete(copy_base_path);

    EXPECT_FALSE(filesys::exists(target));
    EXPECT_NO_THROW(CopyDirectoryRecursively(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    source = target;
    target = filesys::system_complete(alt_move_base_path);
    EXPECT_NO_THROW(MoveDirectoryAndContents(source.wstring(), target.wstring()));
    EXPECT_TRUE(filesys::exists(target));

    EXPECT_TRUE(filesys::remove_all(target) > 0);
    EXPECT_FALSE(filesys::exists(target));
}

#endif // DISABLE_FILEUTILS_TESTS
