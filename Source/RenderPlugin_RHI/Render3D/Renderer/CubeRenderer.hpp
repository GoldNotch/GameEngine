#pragma once
#include <GameFramework.hpp>
#include <OwnedBy.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{
struct Scene3D_GPU;

class CubeRenderer : public RHI::OwnedBy<Scene3D_GPU>
{
public:
  explicit CubeRenderer(Scene3D_GPU & scene);
  ~CubeRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(Scene3D_GPU, GetScene);

public:
  void TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes);
  void Submit();

private:
  size_t m_hash = 0;
  std::vector<GameFramework::Mat4f> m_matricesCpuBuffer;
  RHI::IBufferUniformDescriptor * m_vpDescriptor;
  RHI::ISubpass * m_renderPass = nullptr;
  RHI::IBufferGPU * m_matricesBuffer = nullptr;
};
} // namespace RenderPlugin
