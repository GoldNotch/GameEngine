#include "ScreenDevice.hpp"

#include <Scene2D.hpp>

namespace RenderPlugin
{
ScreenDevice::ScreenDevice(GameFramework::IWindow & window)
  : m_window(window)
{
}

ScreenDevice::~ScreenDevice()
{
  assert(m_renderTarget == nullptr);
}

GameFramework::Scene2DUPtr ScreenDevice::AcquireScene2D()
{
  return std::make_unique<Scene2D>(*this);
}

bool ScreenDevice::BeginFrame()
{
  if (!m_framebuffer)
    return false;
  m_renderTarget = m_framebuffer->BeginFrame();
  return m_renderTarget != nullptr;
}

void ScreenDevice::EndFrame()
{
  m_framebuffer->EndFrame();
  m_renderTarget = nullptr;
}

void ScreenDevice::Refresh()
{
}

const GameFramework::IWindow & ScreenDevice::GetWindow() const & noexcept
{
  return m_window;
}

} // namespace RenderPlugin
