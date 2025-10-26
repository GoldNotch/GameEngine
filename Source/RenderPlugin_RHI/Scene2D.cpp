#include "Scene2D.hpp"

namespace RenderPlugin
{

Scene2D::Scene2D(RHI::IFramebuffer & framebuffer)
  : m_framebuffer(framebuffer)
  , m_renderPass(framebuffer.CreateSubpass())
{
  auto && subpassConfig = m_renderPass->GetConfiguration();
  subpassConfig.BindAttachment(0, RHI::ShaderAttachmentSlot::Color);
  subpassConfig.BindAttachment(1, RHI::ShaderAttachmentSlot::DepthStencil);
  subpassConfig.BindResolver(2, 0);
  subpassConfig.EnableDepthTest(true);
}

Scene2D::~Scene2D()
{
  //TODO: destroy renderPass
}

void Scene2D::AddBackground(const std::array<float, 4> & color)
{
}

void Scene2D::AddRect(float left, float top, float right, float bottom)
{
}

void Scene2D::Invalidate()
{
  //TODO: m_renderPass->SetDirtyCommands();
}

void Scene2D::Draw()
{
  m_renderPass->BeginPass();
  auto extent = m_framebuffer.GetExtent();
  uint32_t width = extent[0], height = extent[1];
  m_renderPass->SetViewport(static_cast<float>(width), static_cast<float>(height));
  m_renderPass->SetScissor(0, 0, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

  //m_renderPass->BindVertexBuffer(0, *m_cubesBuffer);
  //m_renderPass->DrawVertices(static_cast<uint32_t>(m_cubesCount), 1);

  m_renderPass->EndPass();
}

bool Scene2D::ShouldBeInvalidated() const noexcept
{
  return m_renderPass->ShouldBeInvalidated();
}

} // namespace RenderPlugin
