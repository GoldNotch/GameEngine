#pragma once
#include <memory>

#include <Render/Material.hpp>
#include <Render3D/RenderData.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{

struct MaterialResolver
{
  explicit MaterialResolver(RHI::ISubpassConfiguration & config,
                            const std::filesystem::path & fragmentShader);
  virtual ~MaterialResolver() = default;

  TextureSlots AddMaterialForInstance(const GameFramework::Material * material);

private:
  std::vector<RHI::ISamplerUniformDescriptor *> m_descriptors;
};

} // namespace RenderPlugin
