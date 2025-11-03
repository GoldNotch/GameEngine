#pragma once
#include <GameFramework_def.h>

#include <filesystem>

#include <Files/MountPoint.hpp>

namespace GameFramework
{

struct IFileManager
{
  virtual ~IFileManager() = default;
  /// add new mount point into manager
  virtual void Mount(std::filesystem::path shortPath, MountPointUPtr && mountPoint) = 0;
  /// open file for reading
  virtual FileReaderUPtr OpenRead(const std::filesystem::path & path) const = 0;
  /// open file for writing
  virtual FileWriterUPtr OpenWrite(const std::filesystem::path & path) const = 0;
};

GAME_FRAMEWORK_API IFileManager & GetFileManager() noexcept;

} // namespace GameFramework
