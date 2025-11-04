#pragma once

#include <PluginInterfaces/RenderPlugin.hpp>
#include <Renderer/BackgroundRenderer.hpp>

namespace RenderPlugin
{

struct Scene2D : public GameFramework::IRenderableScene2D
{
  explicit Scene2D(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer);
  virtual ~Scene2D() override;

  virtual void AddBackground(const std::array<float, 4> & color) override;
  virtual void AddRect(float left, float top, float right, float bottom) override;

public:
  void Invalidate();
  void Draw();
  bool ShouldBeInvalidated() const noexcept;

private:
  BackgroundRenderer m_backgroundRenderer;
};

} // namespace RenderPlugin
