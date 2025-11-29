#include "Scene3D_GPU.hpp"

namespace RenderPlugin
{
Scene3D_GPU::Scene3D_GPU(InternalDevice & device)
  : OwnedBy<InternalDevice>(device)
  , m_viewProjBuffer(
      device.GetContext().AllocBuffer(sizeof(ViewProjection), RHI::UniformBuffer, true))
{
}

Scene3D_GPU::~Scene3D_GPU()
{
  //TODO: delete m_viewProjBuffer
}

void Scene3D_GPU::TrySetCubes(RenderableBatches<GameFramework::Cube> && batches)
{
  for (auto && [shaderPath, batch] : batches)
  {
    auto [it, inserted] = m_cubeRenderers.insert({shaderPath, nullptr});
    if (inserted)
      it->second = std::make_unique<CubeRenderer>(*this, shaderPath);
    it->second->TrySetCubes(batch.first, batch.second);
  }
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
  for (auto && [matClass, renderer] : m_cubeRenderers)
    renderer->Submit();
}

bool Scene3D_GPU::ShouldBeInvalidated() const noexcept
{
  return false;
}
} // namespace RenderPlugin
