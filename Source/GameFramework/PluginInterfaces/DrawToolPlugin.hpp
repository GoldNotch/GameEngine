#pragma once
#include <array>

#include <Plugin/Plugin.hpp>

namespace GameFramework
{

/// Инструмент для рисования. Предоставляет высокоуровневые операции для рисования
struct DrawTool
{
  virtual ~DrawTool() = default;
  virtual void SetClearColor(const std::array<float, 4> & color) = 0;
  //virtual void SetViewport(float left, float top, uint32_t width, uint32_t height) = 0;
  virtual void Flush() = 0;
  //virtual void DrawCube(/*transform*/) = 0;
  //virtual void DrawSphere(/*transform*/) = 0;
  virtual void DrawRect(float left, float top, float right, float bottom) = 0;
};

struct ScreenDevice
{
  virtual bool BeginFrame() = 0;
  virtual void EndFrame() = 0;
};

struct RenderPlugin : public IPluginInstance
{
  virtual ~RenderPlugin() = default;
  virtual std::unique_ptr<DrawTool> CreateDrawTool() = 0;
  virtual std::unique_ptr<ScreenDevice> CreateScreenDevice(DrawTool & drawTool);
};

} // namespace GameFramework
