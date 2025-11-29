#include "MaterialResolver.hpp"

#include <typeindex>
#include <unordered_map>

#include <Files/FileManager.hpp>
#include <Utility/Constants.hpp>
#include <Utility/ShaderFile.hpp>

namespace RenderPlugin
{

MaterialResolver::MaterialResolver(RHI::ISubpassConfiguration & config,
                                   const std::filesystem::path & fragmentShader)
{
  auto && stream = GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "Cube_frag.spv");
  ShaderFile file;
  stream->ReadValue<ShaderFile>(file);
  config.AttachShader(RHI::ShaderType::Fragment, file.GetSpirV());
  std::vector<RHI::ISamplerUniformDescriptor *> descriptors;
  descriptors.resize(10, nullptr);
  //config.DeclareSamplersArray({0, 0}, RHI::ShaderType::Fragment, 10, descriptors.data());
}

TextureSlots MaterialResolver::AddMaterialForInstance(const GameFramework::Material * material)
{
  return UnusedSlots;
}

} // namespace RenderPlugin
