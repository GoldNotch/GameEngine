#pragma once
#include <unordered_set>

#include <Files/MountPoint.hpp>

namespace RenderPlugin
{

class ShadersMountPoint final : public GameFramework::IMountPoint
{
public:
  explicit ShadersMountPoint(const std::filesystem::path & rootPath);
  ~ShadersMountPoint() override = default;

public:
  /// path to mount point
  virtual const std::filesystem::path & Path() const & noexcept override;
  /// Checks that file exists in mount point
  virtual bool Exists(const std::filesystem::path & path) const override;
  /// Open file stream for reading or writing
  virtual GameFramework::FileStreamUPtr Open(const std::filesystem::path & path) override;
  /// enumerates all files and returns paths
  virtual std::vector<std::filesystem::path> ListFiles(
    const std::filesystem::path & rootPath = "") const override;

private:
  std::filesystem::path m_rootPath;
  std::unordered_set<std::filesystem::path> m_shaders;
};

} // namespace RenderPlugin
