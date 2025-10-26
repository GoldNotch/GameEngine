#pragma once
#include <GameFramework.hpp>

namespace RenderPlugin
{
struct Scene2D;

struct Scene2DProxy final : public GameFramework::IRenderableScene2D
{
  explicit Scene2DProxy(Scene2D & scene);
  virtual ~Scene2DProxy() override;

  virtual void AddBackground(const std::array<float, 4> & color) override;
  virtual void AddRect(float left, float top, float right, float bottom) override;

private:
  Scene2D * m_scene = nullptr;
};
} // namespace RenderPlugin
