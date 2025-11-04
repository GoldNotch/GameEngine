#pragma once
#include <GameFramework.hpp>

namespace RenderPlugin
{
struct Scene2D_GPU;

struct Scene2D_CPU final : public GameFramework::IRenderableScene2D
{
  explicit Scene2D_CPU(Scene2D_GPU & scene);
  virtual ~Scene2D_CPU() override;

  virtual void SetBackground(const GameFramework::Color3f & color) override;
  virtual void AddRect(const GameFramework::Rect2d & rect) override;

private:
  Scene2D_GPU * m_boundScene = nullptr;
  std::vector<GameFramework::Rect2d> m_rectsToDraw;
  size_t m_rectsHash = 0;
};
} // namespace RenderPlugin
