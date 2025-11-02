#include "ShaderFile.hpp"

#include <cassert>
#include <fstream>
#include <span>

namespace RenderPlugin
{

ShaderFile ShaderFile::Read(GameFramework::IFileStream & stream)
{
  ShaderFile result;
  //result.m_data.resize(stream.Size() / sizeof(uint32_t) + 1);
  std::span<uint32_t> dataSpan(result.m_data.data(), result.m_data.size());
  //stream.Read(std::as_writable_bytes(dataSpan));
  return result;
}

void ShaderFile::Write(const ShaderFile & file, GameFramework::IFileStream & stream)
{
  assert(false); // You can't write shaders
}

} // namespace RenderPlugin
