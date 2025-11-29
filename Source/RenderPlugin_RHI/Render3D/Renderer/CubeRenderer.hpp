#pragma once
#include <GameFramework.hpp>
#include <OwnedBy.hpp>
#include <RHI.hpp>
#include <Utility/MaterialResolver.hpp>

namespace RenderPlugin
{
struct Scene3D_GPU;

class CubeRenderer : public RHI::OwnedBy<Scene3D_GPU>
{
public:
  explicit CubeRenderer(Scene3D_GPU & scene, const std::filesystem::path & materialShader);
  ~CubeRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(Scene3D_GPU, GetScene);

public:
  void TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes);
  void Submit();

private:
  RHI::ISubpass * m_renderPass = nullptr;
  MaterialResolver m_materialResolver;
  size_t m_hash = 0;
  std::vector<GameFramework::Mat4f> m_matricesCpuBuffer; // TODO: remove
  std::vector<TextureSlots> m_texturesCpuBuffer; // TODO: remove
  RHI::IBufferUniformDescriptor * m_vpDescriptor;
  RHI::IBufferGPU * m_matricesBuffer = nullptr;
  RHI::IBufferGPU * m_texturesBuffer = nullptr;
};
} // namespace RenderPlugin
