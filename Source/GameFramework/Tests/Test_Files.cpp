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


TEST_CASE("", "[FileManager]")
{
  GetFileManager().Mount("data", CreateDirectoryMountPoint(testDir1));
  GetFileManager().Mount("data/meshes", CreateDirectoryMountPoint(testDir2));
  GetFileManager().Mount("data/scripts", CreateDirectoryMountPoint(testDir3));

  auto notFountStream = GetFileManager().Open("data/unknown.txt");
  REQUIRE(notFountStream == nullptr);

  auto stream1 = GetFileManager().Open("data/file1.dat");
  REQUIRE(stream1 != nullptr);
  std::string dummyString = "text1: dummy string";
  stream1->Write(dummyString);
  std::string readString(dummyString.size(), '\0');
  size_t readBytes = stream1->Read(readString);
  REQUIRE(readString == dummyString);
}
