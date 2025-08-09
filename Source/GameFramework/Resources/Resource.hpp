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
  /// Upload resource to RAM. return 0 if data is not uploaded yet, otherwise returns hash of uploaded data
  virtual size_t Upload() = 0;
  /// Free resource from RAM
  virtual void Free() = 0;

private:
  std::filesystem::path m_path;
};

inline size_t CalcResourceTimestamp()
{
  // Get current time since epoch in seconds
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  return static_cast<size_t>(seconds);
}

} // namespace GameFramework
