#pragma once
#include <memory>

#include <Render/Color.hpp>

namespace GameFramework
{
struct IRenderableScene2D
{
  //destructor will draw objects
  virtual ~IRenderableScene2D() = default;
  virtual void SetBackground(const Color3f & color) = 0;
  virtual void AddRect(const Rect2d & rect) = 0;
};

using Scene2DUPtr = std::unique_ptr<IRenderableScene2D>;

} // namespace GameFramework
