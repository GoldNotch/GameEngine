#pragma once
#include <list>

#include <GameFramework.hpp>
#include <RHI.hpp>
#include <Scene2D_GPU.hpp>

namespace RenderPlugin
{

struct ScreenDevice : public GameFramework::IScreenDevice
{
  explicit ScreenDevice(RHI::IContext & ctx, GameFramework::IWindow & window);
  virtual ~ScreenDevice() override;

public: // IDevice interface
  virtual GameFramework::Scene2DUPtr AcquireScene2D() override;
  virtual int GetOwnerId() const noexcept override;

public: //IScreenDevice interface
  virtual bool BeginFrame() override;
  virtual void EndFrame() override;
  virtual void Refresh() override;
  virtual const GameFramework::IWindow & GetWindow() const & noexcept override;

private:
  RHI::IContext & m_context;
  GameFramework::IWindow & m_window;
  RHI::IFramebuffer * m_framebuffer = nullptr;
  RHI::IRenderTarget * m_renderTarget = nullptr;
  RHI::IAttachment * m_colorAttachment = nullptr;
  RHI::IAttachment * m_depthStencilAttachment = nullptr;
  RHI::IAttachment * m_msaaResolveAttachment = nullptr;

  Scene2D_GPU m_scene2D;
};

} // namespace RenderPlugin
