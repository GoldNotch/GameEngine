#pragma once
#include <list>

#include <GameFramework.hpp>
#include <InternalDeviceInterface.hpp>
#include <Render2D/Scene2D_GPU.hpp>
#include <Render3D/Scene3D_GPU.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{

struct ScreenDevice : public GameFramework::IScreenDevice,
                      public InternalDevice
{
  explicit ScreenDevice(RHI::IContext & ctx, GameFramework::IWindow & window);
  virtual ~ScreenDevice() override;

public: // IDevice interface
  virtual GameFramework::Scene2DUPtr AcquireScene2D() override;
  virtual GameFramework::Scene3DUPtr AcquireScene3D() override;
  virtual int GetOwnerId() const noexcept override;
  virtual float GetAspectRatio() const noexcept override;

public: //IScreenDevice interface
  virtual bool BeginFrame() override;
  virtual void EndFrame() override;
  virtual void Refresh() override;
  virtual void OnResize(int newWidth, int newHeight) override;
  virtual const GameFramework::IWindow & GetWindow() const & noexcept override;

public: // internal device
  virtual void ConfigurePipeline(RHI::ISubpassConfiguration & config) const override;
  virtual RHI::IFramebuffer& GetFramebuffer() & noexcept override;

private:
  GameFramework::IWindow & m_window;
  RHI::IFramebuffer * m_framebuffer = nullptr;
  RHI::IRenderTarget * m_renderTarget = nullptr;
  RHI::IAttachment * m_colorAttachment = nullptr;
  RHI::IAttachment * m_depthStencilAttachment = nullptr;
  RHI::IAttachment * m_msaaResolveAttachment = nullptr;

  Scene2D_GPU m_scene2D;
  Scene3D_GPU m_scene3D;
};

} // namespace RenderPlugin
