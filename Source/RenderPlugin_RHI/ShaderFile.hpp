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
  static size_t ReadBinary(GameFramework::IFileReader & stream, ShaderFile & file);
  static void WriteBinary(GameFramework::IFileWriter & stream, const ShaderFile & file);

private:
  std::vector<uint32_t> m_data;
};

} // namespace RenderPlugin
