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
  virtual size_t Read(std::span<char> buffer) override;
  /// write bytes into stream
  virtual void Write(std::span<const char> data) override;

private:
  std::fstream m_stream;
};


StandardFileStream::StandardFileStream(const std::filesystem::path & path, bool binary)
{
  std::ios_base::openmode mode = std::ios::in | std::ios::out | std::ios::trunc;
  if (binary)
    mode |= std::ios_base::binary;
  m_stream.open(path, mode);
  if (m_stream.is_open() && m_stream.good())
  {
  }
  else
  {
    throw std::runtime_error("Failed to open file");
  }
}

size_t StandardFileStream::Read(std::span<char> buffer)
{
  m_stream.read(buffer.data(), buffer.size_bytes());
  return m_stream.gcount();
}

void StandardFileStream::Write(std::span<const char> data)
{
  m_stream.write(data.data(), data.size_bytes());
}


GAME_FRAMEWORK_API FileStreamUPtr OpenBinaryFileStream(const std::filesystem::path & path)
{
  return std::make_unique<StandardFileStream>(path, true /*binary*/);
}


//GAME_FRAMEWORK_API FileStreamUPtr OpenTextFileStream(const std::filesystem::path & path)
//{
//  return std::make_unique<StandardFileStream>(path, false /*binary*/);
//}
} // namespace GameFramework
