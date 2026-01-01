#include "Scene3D_GPU.hpp"

namespace RenderPlugin
{
Scene3D_GPU::Scene3D_GPU(InternalDevice & device)
  : OwnedBy<InternalDevice>(device)
  , m_viewProjBuffer(
      device.GetContext().AllocBuffer(sizeof(ViewProjection), RHI::UniformBuffer, true))
  , m_cubesRenderer(*this)
{
}

Scene3D_GPU::~Scene3D_GPU()
{
  //TODO: delete m_viewProjBuffer
}

void Scene3D_GPU::TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes)
{
  m_cubesRenderer.TrySetCubes(newHash, cubes);
}

void Scene3D_GPU::SetCamera(const GameFramework::Camera & camera)
{
  ViewProjection vp{camera.GetViewMatrix(), camera.GetProjectionMatrix()};
  m_viewProjBuffer->UploadSync(&vp, sizeof(vp));
}

RHI::IBufferGPU * Scene3D_GPU::GetViewProjectionBuffer()
{
  return m_viewProjBuffer;
}

void Scene3D_GPU::Invalidate()
{
  //TODO: m_renderPass->SetDirtyCommands();
}

void Scene3D_GPU::Draw()
{
  m_cubesRenderer.Submit();
}

bool Scene3D_GPU::ShouldBeInvalidated() const noexcept
{
  return false;
}
} // namespace RenderPlugin
