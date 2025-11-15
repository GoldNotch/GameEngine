#include <cassert>
#include <fstream>
#include <stdexcept>

#include "FileStream.hpp"

namespace GameFramework
{
class StandardFileReader : public IFileReader
{
public:
  explicit StandardFileReader(const std::filesystem ::path & path);
  virtual ~StandardFileReader() = default;

public: // IFileReader
  virtual std::filesystem::path FullPath() const override { return m_path; }
  /// read bytes from stream
  virtual size_t Read(std::span<std::byte> buffer) override;
  /// get position of caret in file
  virtual size_t Tell() override { return m_stream.tellg(); }
  /// move writing caret in the file
  virtual void Seek(std::ptrdiff_t offset, SeekDirection dir) override;
  /// get size of file
  virtual size_t Size() const override { return m_fileSize; }

private:
  std::filesystem::path m_path;
  std::ifstream m_stream;
  size_t m_fileSize = 0;
};


StandardFileReader::StandardFileReader(const std::filesystem::path & path)
  : m_path(path)
{
  std::ios_base::openmode mode = std::ios::in | std::ios_base::binary | std::ios::ate;
  m_stream.open(path, mode);
  if (m_stream.is_open() && m_stream.good())
  {
    m_fileSize = m_stream.tellg();
    m_stream.seekg(0, std::ios::beg);
  }
  else
  {
    throw std::runtime_error("Failed to open file");
  }
}

size_t StandardFileReader::Read(std::span<std::byte> buffer)
{
  m_stream.read(reinterpret_cast<char *>(buffer.data()), buffer.size_bytes());
  return m_stream.gcount();
}

void StandardFileReader::Seek(std::ptrdiff_t offset, SeekDirection dir)
{
  m_stream.seekg(offset, static_cast<std::ios::seekdir>(dir));
}

struct StandardFileWriter : public IFileWriter
{
  explicit StandardFileWriter(const std::filesystem::path & path);
  virtual ~StandardFileWriter() = default;

public:
  virtual std::filesystem::path FullPath() const override { return m_path; }
  /// write bytes into stream
  virtual void Write(std::span<const std::byte> data) override;
  /// get position of caret in file
  virtual size_t Tell() override { return m_stream.tellp(); }
  /// move writing caret in the file
  virtual void Seek(std::ptrdiff_t offset, SeekDirection dir) override;
  /// finish all writing operations
  virtual void Flush() override { m_stream.flush(); }

private:
  std::filesystem::path m_path;
  std::ofstream m_stream;
};

StandardFileWriter::StandardFileWriter(const std::filesystem::path & path)
  : m_path(path)
  , m_stream(path, std::ios::out | std::ios::binary)
{
  if (!m_stream.is_open() || m_stream.bad())
    throw std::runtime_error("Failed to open file");
}

void StandardFileWriter::Write(std::span<const std::byte> data)
{
  m_stream.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
}

void StandardFileWriter::Seek(std::ptrdiff_t offset, SeekDirection dir)
{
  m_stream.seekp(offset, static_cast<std::ios::seekdir>(dir));
}


GAME_FRAMEWORK_API FileWriterUPtr OpenBinaryFileWrite(const std::filesystem::path & path)
{
  return std::make_unique<StandardFileWriter>(path);
}


GAME_FRAMEWORK_API FileReaderUPtr OpenBinaryFileRead(const std::filesystem::path & path)
{
  return std::make_unique<StandardFileReader>(path);
}


//GAME_FRAMEWORK_API FileStreamUPtr OpenTextFileStream(const std::filesystem::path & path)
//{
//  return std::make_unique<StandardFileReader>(path, false /*binary*/);
//}
} // namespace GameFramework
