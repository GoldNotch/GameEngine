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
  /// open file for reading or writing
  virtual FileStreamUPtr Open(const std::filesystem::path & path) const = 0;
};

GAME_FRAMEWORK_API IFileManager & GetFileManager() noexcept;

} // namespace GameFramework
