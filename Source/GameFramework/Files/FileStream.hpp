#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>
#include <span>
#include <type_traits>

namespace GameFramework
{

enum class SeekDirection
{
  Begin,
  Current,
  End
};

struct IFileStream
{
  virtual ~IFileStream() = default;
  virtual std::filesystem::path FullPath() const = 0;
  /// get position of caret in file
  virtual size_t Tell() = 0;
  /// move writing caret in the file
  virtual void Seek(std::ptrdiff_t offset, SeekDirection dir) = 0;
};

struct IFileReader : public IFileStream
{
  virtual ~IFileReader() = default;
  /// read bytes from stream
  virtual size_t Read(std::span<std::byte> buffer) = 0;
  /// get size of file
  virtual size_t Size() const = 0;

  template<typename T>
  size_t ReadValue(T & val);

  /// reads trivially copyable value from stream
  template<typename T>
    requires(std::is_trivial_v<T>)
  size_t ReadValue(T & value)
  {
    return Read(std::as_writable_bytes(std::span{&value, 1}));
  }

  template<typename StrT>
    requires(std::is_convertible_v<StrT, std::string_view> ||
             std::is_convertible_v<StrT, std::wstring_view>)
  size_t ReadValue(StrT & val)
  {
    return Read(std::as_writable_bytes(std::span{val}));
  }

  template<typename T>
    requires requires(IFileReader & s, T & v) { T::ReadBinary(s, v); }
  size_t ReadValue(T & value)
  {
    return T::ReadBinary(*this, value);
  }
};

struct IFileWriter : public IFileStream
{
  virtual ~IFileWriter() = default;
  /// write bytes into stream
  virtual void Write(std::span<const std::byte> data) = 0;
  /// finish all writing operations
  virtual void Flush() = 0;

  template<typename T>
  void WriteValue(const T & value);

  template<typename T>
    requires(std::is_trivial_v<T>)
  void WriteValue(const T & value)
  {
    Write(std::as_bytes(std::span{&value, 1}));
  }

  template<typename T>
    requires requires(IFileWriter & s, const T & v) { T::WriteBinary(s, v); }
  void WriteValue(const T & object)
  {
    T::WriteBinary(*this, object);
  }

  template<typename StrT>
    requires(std::is_convertible_v<StrT, std::string_view> ||
             std::is_convertible_v<StrT, std::wstring_view>)
  void WriteValue(const StrT & value)
  {
    Write(std::as_bytes(std::span{value}));
  }
};

using FileReaderUPtr = std::unique_ptr<IFileReader>;
using FileWriterUPtr = std::unique_ptr<IFileWriter>;

} // namespace GameFramework

namespace GameFramework
{

GAME_FRAMEWORK_API FileWriterUPtr OpenBinaryFileWrite(const std::filesystem::path & path);
GAME_FRAMEWORK_API FileReaderUPtr OpenBinaryFileRead(const std::filesystem::path & path);
// blocks OpenTextFileStream because std::fstream::write can't write a text data for integer
//GAME_FRAMEWORK_API FileStreamUPtr OpenTextFileStream(const std::filesystem::path & path);

} // namespace GameFramework
