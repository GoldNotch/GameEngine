#pragma once
#include <vector>

#include <Files/FileStream.hpp>

namespace RenderPlugin
{

class ShaderFile final
{
public:
  ShaderFile() = default;
  ~ShaderFile() = default;
  const std::vector<uint32_t> & GetSpirV() const & noexcept { return m_data; }

public:
  static ShaderFile Read(GameFramework::IFileStream & stream);
  static void Write(const ShaderFile & file, GameFramework::IFileStream & stream);

private:
  std::vector<uint32_t> m_data;
};

} // namespace RenderPlugin
