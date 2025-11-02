#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>
#include <span>
#include <type_traits>

namespace GameFramework
{

struct IFileStream
{
  virtual ~IFileStream() = default;
  /// read bytes from stream
  virtual size_t Read(std::span<char> buffer) = 0;
  /// write bytes into stream
  virtual void Write(std::span<const char> data) = 0;

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
