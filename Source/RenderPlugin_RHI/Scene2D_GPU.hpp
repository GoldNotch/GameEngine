#pragma once

#include <PluginInterfaces/RenderPlugin.hpp>
#include <Renderer/BackgroundRenderer.hpp>
#include <Renderer/Rect2dRenderer.hpp>

namespace RenderPlugin
{

struct Scene2D_GPU final
{
  explicit Scene2D_GPU(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer);
  ~Scene2D_GPU();

  void SetBackground(const GameFramework::Color3f & color);
  void TrySetRects(size_t newHash, std::span<const GameFramework::Rect2d> rects);

public:
  void Invalidate();
  void Draw();
  bool ShouldBeInvalidated() const noexcept;

private:
  BackgroundRenderer m_backgroundRenderer;
  Rect2DRenderer m_rectsRenderer;
};

} // namespace RenderPlugin
