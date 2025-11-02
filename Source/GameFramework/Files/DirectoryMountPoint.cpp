#include <unordered_set>

#include <Files/MountPoint.hpp>

namespace GameFramework
{

class DirectoryMountPoint : public IMountPoint
{
public:
  explicit DirectoryMountPoint(const std::filesystem::path & path);
  virtual ~DirectoryMountPoint() override = default;

public:
  /// path to mount point
  virtual const std::filesystem::path & Path() const & noexcept override;
  /// Checks that file exists in mount point
  virtual bool Exists(const std::filesystem::path & path) const override;
  /// Open file stream for reading or writing
  virtual FileStreamUPtr Open(const std::filesystem::path & path) override;
  /// enumerates all files and returns paths
  virtual std::vector<std::filesystem::path> ListFiles(
    const std::filesystem::path & rootPath = "") const override;

private:
  std::filesystem::path m_rootPath;
  std::unordered_set<std::filesystem::path> m_files;
};

DirectoryMountPoint::DirectoryMountPoint(const std::filesystem::path & path)
  : m_rootPath(path)
{
  for (auto && entry : std::filesystem::recursive_directory_iterator(m_rootPath))
  {
    if (entry.is_regular_file())
      m_files.emplace(entry.path());
  }
}

const std::filesystem::path & DirectoryMountPoint::Path() const & noexcept
{
  return m_rootPath;
}

bool DirectoryMountPoint::Exists(const std::filesystem::path & path) const
{
  auto correctPath = m_rootPath / path;
  if (m_files.find(correctPath) != m_files.end())
  {
    return std::filesystem::exists(correctPath);
  }
  return false;
}

FileStreamUPtr DirectoryMountPoint::Open(const std::filesystem::path & path)
{
  return OpenBinaryFileStream(m_rootPath / path);
}

std::vector<std::filesystem::path> DirectoryMountPoint::ListFiles(
  const std::filesystem::path & rootPath) const
{
  return std::vector<std::filesystem::path>(m_files.begin(), m_files.end());
}


GAME_FRAMEWORK_API MountPointUPtr CreateDirectoryMountPoint(const std::filesystem::path & path)
{
  return std::make_unique<DirectoryMountPoint>(path);
}

} // namespace GameFramework
