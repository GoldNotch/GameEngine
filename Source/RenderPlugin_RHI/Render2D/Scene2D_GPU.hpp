#pragma once

#include <GameFramework.hpp>
#include <InternalDeviceInterface.hpp>
#include <Render2D/Renderer/BackgroundRenderer.hpp>
#include <Render2D/Renderer/Rect2dRenderer.hpp>

namespace RenderPlugin
{

struct Scene2D_GPU final : public RHI::OwnedBy<InternalDevice>
{
  explicit Scene2D_GPU(InternalDevice & device);
  virtual ~Scene2D_GPU() override;
  MAKE_ALIAS_FOR_GET_OWNER(InternalDevice, GetDevice);

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
