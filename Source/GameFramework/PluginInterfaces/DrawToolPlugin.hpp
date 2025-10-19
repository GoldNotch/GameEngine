#pragma once
#include <array>

#include <Plugin/Plugin.hpp>

namespace GameFramework
{

struct IDrawFuncs2D
{
  virtual ~IDrawFuncs2D() = default;
  virtual void DrawRect(float left, float top, float right, float bottom) = 0;
};

/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct IDrawTool : public IPluginInstance,
                   public IDrawFuncs2D
{
  virtual ~IDrawTool() = default;
  virtual void SetClearColor(const std::array<float, 4> & color) = 0;
  //virtual void SetViewport(float left, float top, uint32_t width, uint32_t height) = 0;
  virtual void Flush() = 0;
  //virtual void DrawCube(/*transform*/) = 0;
  //virtual void DrawSphere(/*transform*/) = 0;
};

} // namespace GameFramework
