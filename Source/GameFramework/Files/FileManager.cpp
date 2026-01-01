#include "FileManager.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <map>
#include <ranges>
#include <stdexcept>

namespace
{
size_t PathPrefixLength(const std::filesystem::path & dir,
                        const std::filesystem::path & subdir) noexcept
{
  size_t result = 0;
  auto it1 = dir.begin();
  auto it2 = subdir.begin();
  while ((it1 != dir.end() || it2 != subdir.end()) && (*it1 == *it2))
  {
    it1++;
    it2++;
    result++;
  }
  return result;
}
} // namespace

namespace GameFramework
{
class FileManagerImpl : public IFileManager
{
public:
  FileManagerImpl();

public:
  virtual void Mount(std::filesystem::path shortPath, MountPointUPtr && mountPoint) override;
  /// open file for reading
  virtual FileReaderUPtr OpenRead(const std::filesystem::path & path) const override;
  /// open file for writing
  virtual FileWriterUPtr OpenWrite(const std::filesystem::path & path) const override;

private:
  template<typename ResultT>
  ResultT OpenImpl(const std::filesystem::path & path) const;

private:
  using MountOverrides = std::vector<MountPointUPtr>; // it behaves like a stack
  std::map<std::filesystem::path, MountOverrides> m_mountedPoints;
};


FileManagerImpl::FileManagerImpl()
{
  Mount("/", CreateDirectoryMountPoint(std::filesystem::current_path()));
}

void FileManagerImpl::Mount(std::filesystem::path shortPath, MountPointUPtr && mountPoint)
{
  auto [it, inserted] = m_mountedPoints.insert({shortPath, MountOverrides{}});
  // all mount points should be with unique path
  auto subIt = std::ranges::find_if(it->second, [&mountPoint](const MountPointUPtr & mp)
                                    { return mp->Path() == mountPoint->Path(); });
  if (subIt == it->second.end())
    it->second.emplace_back(std::move(mountPoint));
  else
    *subIt = std::move(mountPoint);
}


template<typename ResultT>
ResultT FileManagerImpl::OpenImpl(const std::filesystem::path & path) const
{
  if (path.empty())
    throw std::runtime_error("Invalid path");

  size_t longestPrefix = 0;
  const MountOverrides * overrideWithLongestPrefix = nullptr;
  std::filesystem::path::iterator pathBegin = path.begin();
  for (auto && [key, overrides] : m_mountedPoints)
  {
    size_t prefix = PathPrefixLength(path, key);
    if (longestPrefix < prefix)
    {
      overrideWithLongestPrefix = &overrides;
      std::advance(pathBegin, prefix - longestPrefix);
      longestPrefix = prefix;
    }
  }

  if (!overrideWithLongestPrefix)
  {
    auto it = m_mountedPoints.find("/");
    overrideWithLongestPrefix = &it->second;
  }

  std::filesystem::path miniPath;
  while (pathBegin != path.end())
  {
    miniPath.append(pathBegin->wstring());
    pathBegin++;
  }
  auto reversedView = *overrideWithLongestPrefix | std::views::reverse;
  for (auto && mountPoint : reversedView)
  {
    if (mountPoint->Exists(miniPath))
    {
      if constexpr (std::is_same_v<ResultT, FileReaderUPtr>)
        return mountPoint->OpenRead(miniPath);
      else
        return mountPoint->OpenWrite(miniPath);
    }
  }

  return nullptr;
}

FileReaderUPtr FileManagerImpl::OpenRead(const std::filesystem::path & path) const
{
  return OpenImpl<FileReaderUPtr>(path);
}

FileWriterUPtr FileManagerImpl::OpenWrite(const std::filesystem::path & path) const
{
  return OpenImpl<FileWriterUPtr>(path);
}


GAME_FRAMEWORK_API IFileManager & GetFileManager() noexcept
{
  static FileManagerImpl s_impl;
  return s_impl;
}

} // namespace GameFramework
