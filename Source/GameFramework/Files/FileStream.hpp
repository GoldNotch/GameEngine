#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>
#include <span>
#include <type_traits>

namespace GameFramework
{

enum class SeekOrigin : uint8_t
{
  Begin,
  Current,
  End
};

struct IFileStream
{
  virtual ~IFileStream() = default;
  /// read bytes from stream
  virtual size_t Read(std::span<std::byte> buffer) = 0;
  /// write bytes into stream
  virtual void Write(std::span<const std::byte> data) = 0;
  /// set cursor in a stream
  virtual bool Seek(ptrdiff_t offset, SeekOrigin origin) = 0;
  /// get position of cursor
  virtual size_t Tell() const = 0;
  /// get size of stream
  virtual size_t Size() const = 0;
  /// checks if end-of-file is reached
  virtual bool Eof() const = 0;

  /// reads trivially copyable value from stream
  template<typename T>
    requires(std::is_trivially_copyable_v<T>)
  T ReadValue()
  {
    T v{};
    Read(std::as_writable_bytes(std::span{&v, 1}));
    return v;
  }

  template<typename T>
    requires(!std::is_trivially_copyable_v<T>) && requires(IFileStream & s) { T::Read(s); }
  T ReadObject()
  {
    return T::Read(*this);
  }

  template<typename T>
    requires(std::is_trivially_copyable_v<T>)
  void WriteValue(const T & value)
  {
    Write(std::as_bytes(std::span{&value, 1}));
  }

  template<typename T>
    requires(!std::is_trivially_copyable_v<T>) &&
            requires(const T & obj, IFileStream & s) { T::Write(obj, s); }
  void WriteObject(const T & object)
  {
    T::Write(object, *this);
  }
};

using FileStreamUPtr = std::unique_ptr<IFileStream>;

} // namespace GameFramework

namespace GameFramework
{

GAME_FRAMEWORK_API FileStreamUPtr OpenBinaryFileStream(const std::filesystem::path & path);
// blocks OpenTextFileStream because std::fstream::write can't write a text data for integer
//GAME_FRAMEWORK_API FileStreamUPtr OpenTextFileStream(const std::filesystem::path & path);

} // namespace GameFramework
