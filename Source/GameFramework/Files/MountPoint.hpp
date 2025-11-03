#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>
#include <vector>

#include <Files/FileStream.hpp>

namespace GameFramework
{
// MountPoint is a just container with files. It could be a directory in disk, or an archive, or remote disk, but it's still a container with files
struct IMountPoint
{
  virtual ~IMountPoint() = default;
  /// path to mount point
  virtual const std::filesystem::path & Path() const & noexcept = 0;
  /// Checks that file exists in mount point
  virtual bool Exists(const std::filesystem::path & path) const = 0;
  /// Open file stream for reading
  virtual FileReaderUPtr OpenRead(const std::filesystem::path & path) = 0;
  /// Open file stream for writing
  virtual FileWriterUPtr OpenWrite(const std::filesystem::path & path) = 0;
  /// enumerates all files and returns paths
  virtual std::vector<std::filesystem::path> ListFiles(
    const std::filesystem::path & rootPath = "") const = 0;
};

using MountPointUPtr = std::unique_ptr<IMountPoint>;

GAME_FRAMEWORK_API MountPointUPtr CreateDirectoryMountPoint(const std::filesystem::path & path);

} // namespace GameFramework
