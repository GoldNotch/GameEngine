#pragma once
#include <filesystem>

namespace GameFramework
{
struct IResource
{
  explicit IResource(const std::filesystem::path & path)
    : m_path(path)
  {
  }

  virtual ~IResource() = default;
  const std::filesystem::path & GetPath() const & noexcept { return m_path; }
  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const = 0;
  /// Upload resource to RAM
  virtual void Upload() = 0;
  /// Free resource from RAM
  virtual void Free() = 0;

private:
  std::filesystem::path m_path;
};
} // namespace GameFramework
