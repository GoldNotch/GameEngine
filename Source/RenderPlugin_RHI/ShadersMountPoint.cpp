#include "ShadersMountPoint.hpp"

namespace RenderPlugin
{


ShadersMountPoint::ShadersMountPoint(const std::filesystem::path & rootPath)
  : m_rootPath(rootPath)
{
  static constexpr const char * s_spirvExtension = ".xpv";
  //TODO: make utility function to enumarate all files with given extension
  for (auto && entry : std::filesystem::recursive_directory_iterator(rootPath))
  {
    if (entry.is_regular_file() && entry.path().extension() == s_spirvExtension)
      m_shaders.emplace(entry.path());
  }
}

const std::filesystem::path & ShadersMountPoint::Path() const & noexcept
{
  return m_rootPath;
}

bool ShadersMountPoint::Exists(const std::filesystem::path & path) const
{
  return m_shaders.find(path) != m_shaders.end();
}

GameFramework::FileStreamUPtr ShadersMountPoint::Open(const std::filesystem::path & path)
{
  return GameFramework::OpenBinaryFileStream(path);
}

std::vector<std::filesystem::path> ShadersMountPoint::ListFiles(
  const std::filesystem::path & rootPath) const
{
  return std::vector<std::filesystem::path>(m_shaders.begin(), m_shaders.end());
}

} // namespace RenderPlugin
