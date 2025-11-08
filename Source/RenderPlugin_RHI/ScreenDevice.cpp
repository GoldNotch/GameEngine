#include "ScreenDevice.hpp"

#include <Scene2D_CPU.hpp>

namespace RenderPlugin
{
ScreenDevice::ScreenDevice(RHI::IContext & ctx, GameFramework::IWindow & window)
  : m_context(ctx)
  , m_window(window)
  , m_framebuffer(ctx.CreateFramebuffer())
  , m_scene2D(ctx, *m_framebuffer)
{
  auto [hwnd, hInstance] = window.GetSurface();
  RHI::SurfaceConfig config{hwnd, hInstance};
  m_msaaResolveAttachment = ctx.CreateSurfacedAttachment(config, RHI::RenderBuffering::Triple);
  auto description = m_msaaResolveAttachment->GetDescription();
  m_colorAttachment = ctx.AllocAttachment(description.format, description.extent,
                                          RHI::RenderBuffering::Triple, RHI::SamplesCount::Eight);
  m_depthStencilAttachment = ctx.AllocAttachment(RHI::ImageFormat::DEPTH_STENCIL,
                                                 description.extent, RHI::RenderBuffering::Triple,
                                                 RHI::SamplesCount::Eight);

  m_framebuffer->AddAttachment(0, m_colorAttachment);
  m_framebuffer->AddAttachment(1, m_depthStencilAttachment);
  m_framebuffer->AddAttachment(2, m_msaaResolveAttachment);
}

ScreenDevice::~ScreenDevice()
{
  assert(m_renderTarget == nullptr);
  //TODO: destroy m_framebuffer in context
}

GameFramework::Scene2DUPtr ScreenDevice::AcquireScene2D()
{
  return std::make_unique<Scene2D_CPU>(m_scene2D);
}

int ScreenDevice::GetOwnerId() const noexcept
{
  return GetWindow().GetId();
}

bool ScreenDevice::BeginFrame()
{
  if (!m_framebuffer)
    return false;
  m_renderTarget = m_framebuffer->BeginFrame();
  m_renderTarget->SetClearValue(0, 0.0, 0.0, 0.0, 1.0);
  m_renderTarget->SetClearValue(1, 1.0f, 0);

  return m_renderTarget != nullptr;
}

void ScreenDevice::EndFrame()
{
  m_framebuffer->EndFrame();
  m_renderTarget = nullptr;
}

void ScreenDevice::Refresh()
{
  m_scene2D.Invalidate();
}

void ScreenDevice::OnResize(int newWidth, int newHeight)
{
    m_framebuffer->Resize(newWidth, newHeight);
}

const GameFramework::IWindow & ScreenDevice::GetWindow() const & noexcept
{
  return m_window;
}

} // namespace RenderPlugin
