#pragma once
#include <vector>

#include <OwnedBy.hpp>
#include <SDL3/SDL.h>


namespace GameFramework
{
struct DrawTool_SDL;
struct RenderTarget;

struct Rect
{
  float left, top, right, bottom;
};

struct QuadRenderer final : public OwnedBy<DrawTool_SDL>
{
  explicit QuadRenderer(DrawTool_SDL & drawTool);
  ~QuadRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(DrawTool_SDL, GetDrawTool);

  /// add object to draw cache
  void PushObjectToDraw(const Rect & rect);

  /// submits commands
  void RenderCache(const RenderTarget & renderTarget);
  void RebuildPipeline(const RenderTarget & renderTarget);
  void UploadToGPU();

private:
  std::vector<Rect> m_rectsToDraw; // CPU cache

  SDL_GPUGraphicsPipeline * m_pipeline = nullptr;
  SDL_GPUBuffer * m_gpuData; // GPU cache

  SDL_GPUShader * m_vertexShader;
  SDL_GPUShader * m_fragmentShader;
};
} // namespace GameFramework
