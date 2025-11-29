#include "ShaderFile.hpp"

#include <cassert>
#include <fstream>
#include <span>

namespace RenderPlugin
{

size_t ShaderFile::ReadBinary(GameFramework::IFileReader & stream, ShaderFile & file)
{
  file.m_data.resize(stream.Size() / sizeof(uint32_t));
  return stream.Read(std::as_writable_bytes(std::span{file.m_data}));
}

void ShaderFile::WriteBinary(GameFramework::IFileWriter & stream, const ShaderFile & file)
{
  assert(false); // You can't write shaders
}

} // namespace RenderPlugin
