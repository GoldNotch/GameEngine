#include "FileManager.hpp"

#include <unordered_map>

namespace GameFramework
{
class FileManagerImpl : public IFileManager
{
public:
  FileManagerImpl() = default;

public:
  virtual void Mount(std::filesystem::path shortPath, MountPointUPtr && mountPoint) override;
  virtual FileStreamUPtr Open(const std::filesystem::path & path) const override;

private:
  std::unordered_map<std::filesystem::path, MountPointUPtr> m_mountedPoints;
};


void FileManagerImpl::Mount(std::filesystem::path shortPath, MountPointUPtr && mountPoint)
{
	//TODO: implement
}


FileStreamUPtr FileManagerImpl::Open(const std::filesystem::path & path) const
{
  //TODO: implement
  return FileStreamUPtr();
}


GAME_FRAMEWORK_API IFileManager & GetFileManager() noexcept
{
  static FileManagerImpl s_impl;
  return s_impl;
}

} // namespace GameFramework
