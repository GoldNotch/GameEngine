#include <iostream>

#include <catch2/catch_test_macros.hpp>
#include <Files/FileManager.hpp>
using namespace GameFramework;

struct DummyFileStream : public IFileStream
{
  explicit DummyFileStream(const std::filesystem::path & path) {};
  /// read bytes from stream
  virtual size_t Read(std::span<std::byte> buffer) override
  {
    return std::cin.readsome(reinterpret_cast<char *>(buffer.data()), buffer.size_bytes());
  }
  /// write bytes into stream
  virtual void Write(std::span<const std::byte> data) override
  {
    std::cout.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
  }
  /// set cursor in a stream
  virtual bool Seek(ptrdiff_t offset, SeekOrigin origin) override
  {
    std::cin.seekg(offset, static_cast<std::ios_base::seekdir>(origin));
    std::cout.seekp(offset, static_cast<std::ios_base::seekdir>(origin));
    return true;
  }
  /// get position of cursor
  virtual size_t Tell() const override { return std::cin.tellg(); }
  /// get size of stream
  virtual size_t Size() const override { return static_cast<size_t>(-1); }
  /// checks if end-of-file is reached
  virtual bool Eof() const override { return false; }
};

struct DummyMountPoint : public IMountPoint
{
  /// path to mount point
  virtual const std::filesystem::path & Path() const & noexcept;
  /// Checks that file exists in mount point
  virtual bool Exists(const std::filesystem::path & path) const;
  /// Open file stream for reading or writing
  virtual FileStreamUPtr Open(const std::filesystem::path & path);
  /// enumerates all files and returns paths
  virtual std::vector<std::filesystem::path> ListFiles(
    const std::filesystem::path & rootPath = "") const;

private:
  std::filesystem::path m_path =
    std::filesystem::temp_directory_path() / "EngineModules/DummyModule/";
};

const std::filesystem::path & DummyMountPoint::Path() const & noexcept
{
  return m_path;
}

bool DummyMountPoint::Exists(const std::filesystem::path & path) const
{
  auto && listFiles = ListFiles();
  for (auto && file : listFiles)
    if (file == path)
      return true;
  return false;
}

FileStreamUPtr DummyMountPoint::Open(const std::filesystem::path & path)
{
  return std::make_unique<DummyFileStream>(m_path / path);
}

std::vector<std::filesystem::path> DummyMountPoint::ListFiles(
  const std::filesystem::path & rootPath) const
{
  // the paths are written as relative paths (in m_path)
  return {"data/file1.txt", "data/file2.txt", "data/file3.txt"};
  // the actual paths to files are:
  // /tmp/EngineModules/DummyModule/data/file1.txt
  // /tmp/EngineModules/DummyModule/data/file2.txt
  // /tmp/EngineModules/DummyModule/data/file3.txt
}


TEST_CASE("", "[FileManager]")
{
  GetFileManager().Mount("dummy", std::make_unique<DummyMountPoint>());

  auto notFountStream = GetFileManager().Open("dummy/data/unknown.txt");
  REQUIRE(notFountStream == nullptr);

  auto stream1 = GetFileManager().Open("dummy/data/file1.txt");
  REQUIRE(stream1 != nullptr);
  std::string dummyString = "text1: dummy string";
  stream1->Write({reinterpret_cast<const std::byte *>(dummyString.c_str()), dummyString.size()});
  stream1->Seek(-dummyString.size(), GameFramework::SeekOrigin::Current);
  std::string readString(dummyString.size(), '\0');
  size_t readBytes =
    stream1->Read({reinterpret_cast<std::byte *>(readString.data()), readString.size()});
  //REQUIRE(readBytes == dummyString.size());
}
