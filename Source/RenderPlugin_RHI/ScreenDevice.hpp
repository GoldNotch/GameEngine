#pragma once
#include <GameFramework.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{

struct ScreenDevice : public GameFramework::IScreenDevice
{
  explicit ScreenDevice(GameFramework::IWindow & window);
  virtual ~ScreenDevice() override;

public:
  virtual GameFramework::Scene2DUPtr AcquireScene2D() override;

public:
  virtual bool BeginFrame() override;
  virtual void EndFrame() override;
  virtual void Refresh() override;
  virtual const GameFramework::IWindow & GetWindow() const & noexcept override;

private:
  GameFramework::IWindow & m_window;
  std::unique_ptr<RHI::IFramebuffer> m_framebuffer;
  RHI::IRenderTarget * m_renderTarget = nullptr;
};

} // namespace RenderPlugin
