#pragma once
#include <vector>

#include <SDL3/SDL.h>

namespace GameFramework
{
struct QuadRenderer final
{
  QuadRenderer();
  ~QuadRenderer();

  /// add object to draw cache
  void PushObjectToDraw(float x, float y, float scale_x, float scale_y);
  /// submits commands
  void RenderCache();

private:
  struct Rect
  {
    float x;
    float y;
    float scale_x;
    float scale_y;
  };

  std::vector<Rect> m_rectsToDraw;
  bool m_cacheValid = false;

  SDL_GPUGraphicsPipeline * m_pipeline;
  SDL_GPUBuffer * m_gpuData;
  SDL_GPUCommandBuffer * m_commands;
  
private:
  void UploadToGPU();
};
} // namespace GameFramework
