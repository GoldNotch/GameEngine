#include <filesystem>
#include <fstream>
#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <Files/FileManager.hpp>
using namespace GameFramework;

static const std::filesystem::path testDir1 = "./TestDir";
static const std::filesystem::path testDir2 = "./TestDir/meshes";
static const std::filesystem::path testDir3 = "./AnotherTestDir";

struct FileManagerTestContext
{
  FileManagerTestContext();
  ~FileManagerTestContext();
};

FileManagerTestContext::FileManagerTestContext()
{
  std::filesystem::create_directory(testDir1);
  for (int i = 0; i < 5; ++i)
  {
    std::string fileName = "file" + std::to_string(i) + ".dat";
    std::ofstream f(testDir1 / fileName, std::ios::binary);
  }

  std::filesystem::create_directory(testDir2);
  for (int i = 0; i < 5; ++i)
  {
    std::string fileName = "mesh" + std::to_string(i) + ".dat";
    std::ofstream f(testDir2 / fileName, std::ios::binary);
  }

  std::filesystem::create_directory(testDir3);
  for (int i = 0; i < 5; ++i)
  {
    std::string fileName = "script" + std::to_string(i) + ".scr";
    std::ofstream f(testDir3 / fileName, std::ios::binary);
  }
}

FileManagerTestContext::~FileManagerTestContext()
{
  std::filesystem::remove_all(testDir1);
  // std::filesystem::remove_all(testDir2); //don't need because testDir2 is inside testDir1
  std::filesystem::remove_all(testDir3);
}

static FileManagerTestContext g_context;


TEST_CASE("Read & Write", "[FileManager]")
{
  GetFileManager().Mount("data", CreateDirectoryMountPoint(testDir1));

  std::string dummyString = "text1: dummy string";
  {
    auto stream = GetFileManager().OpenWrite("data/file1.dat");
    REQUIRE(stream != nullptr);
    stream->WriteValue(dummyString);
    stream->Flush();
  }

  {
    auto stream = GetFileManager().OpenRead("data/file1.dat");
    REQUIRE(stream != nullptr);
    std::string readString(dummyString.size(), '\0');
    stream->ReadValue(readString);
    REQUIRE(readString == dummyString);
  }

  {
    std::string testString = "test string: hello world";
    auto writeStream = GetFileManager().OpenWrite("data/file1.dat");
    writeStream->WriteValue(testString);
    writeStream->Flush(); // use flush to other streams see your changes
    auto readStream = GetFileManager().OpenRead("data/file1.dat");
    REQUIRE(readStream != nullptr);
    REQUIRE(writeStream != nullptr);
    std::string readString(1024, '\0');
    size_t t = readStream->ReadValue(readString);
    REQUIRE(readString.starts_with(testString));
  }
}

TEST_CASE("Open Files", "[FileManager]")
{
  GetFileManager().Mount("data", CreateDirectoryMountPoint(testDir1));
  GetFileManager().Mount("data/meshes", CreateDirectoryMountPoint(testDir2));
  GetFileManager().Mount("data/scripts", CreateDirectoryMountPoint(testDir3));

  auto notFountStream = GetFileManager().OpenRead("data/unknown.txt");
  REQUIRE(notFountStream == nullptr);

  {
    auto stream = GetFileManager().OpenRead("data/file1.dat");
    REQUIRE(stream != nullptr);
    REQUIRE(stream->FullPath() == "./TestDir/file1.dat");
  }

  {
    auto stream = GetFileManager().OpenRead("data/meshes/mesh1.dat");
    REQUIRE(stream != nullptr);
    REQUIRE(stream->FullPath() == "./TestDir/meshes/mesh1.dat");
  }

  {
    auto stream = GetFileManager().OpenRead("data/scripts/script1.scr");
    REQUIRE(stream != nullptr);
    REQUIRE(stream->FullPath() == "./AnotherTestDir/script1.scr");
  }
}
