#include <cassert>
#include <fstream>
#include <stdexcept>

#include "FileStream.hpp"

namespace GameFramework
{
class StandardFileStream : public IFileStream
{
public:
  explicit StandardFileStream(const std::filesystem ::path & path, bool binary);
  virtual ~StandardFileStream() = default;

public:
  /// read bytes from stream
  virtual size_t Read(std::span<std::byte> buffer) override;
  /// write bytes into stream
  virtual void Write(std::span<const std::byte> data) override;
  /// set cursor in a stream
  virtual bool Seek(ptrdiff_t offset, SeekOrigin origin) override;
  /// get position of cursor
  virtual size_t Tell() const override;
  /// get size of stream
  virtual size_t Size() const override;
  /// checks if end-of-file is reached
  virtual bool Eof() const override;

private:
  std::fstream m_stream;
  std::size_t m_fileSize = 0;
};


StandardFileStream::StandardFileStream(const std::filesystem::path & path, bool binary)
{
  std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::trunc | std::ios::ate;
  if (binary)
    mode |= std::ios_base::binary;
  m_stream.open(path, mode);
  if (m_stream.is_open() && m_stream.good())
  {
    m_fileSize = m_stream.tellg();
    m_stream.seekg(0, std::ios::beg);
    m_stream.seekp(0, std::ios::beg);
  }
  else
  {
    throw std::runtime_error("Failed to open file");
  }
}

size_t StandardFileStream::Read(std::span<std::byte> buffer)
{
  return m_stream.readsome(reinterpret_cast<char *>(buffer.data()), buffer.size_bytes());
}

void StandardFileStream::Write(std::span<const std::byte> data)
{
  m_stream.write(reinterpret_cast<const char *>(data.data()), data.size_bytes());
}

bool StandardFileStream::Seek(ptrdiff_t offset, SeekOrigin origin)
{
  std::ios_base::seekdir dir = static_cast<std::ios_base::seekdir>(origin);
  m_stream.seekg(offset, dir);
  m_stream.seekp(offset, dir);
  return true;
}

size_t StandardFileStream::Tell() const
{
  return const_cast<std::fstream &>(m_stream).tellg();
}

size_t StandardFileStream::Size() const
{
  return m_fileSize;
}

bool StandardFileStream::Eof() const
{
  return m_stream.eof();
}


GAME_FRAMEWORK_API FileStreamUPtr OpenBinaryFileStream(const std::filesystem::path & path)
{
  return std::make_unique<StandardFileStream>(path, true /*binary*/);
}


GAME_FRAMEWORK_API FileStreamUPtr OpenTextFileStream(const std::filesystem::path & path)
{
  return std::make_unique<StandardFileStream>(path, false /*binary*/);
}
} // namespace GameFramework
