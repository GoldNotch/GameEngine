#pragma once
#include <vector>

#include <SDL3/SDL.h>

namespace GameFramework
{

struct Vertex
{
  float x, y;
};

struct Rect
{
  float left, top, right, bottom;
};

struct QuadRenderer final
{
  QuadRenderer(SDL_GPUDevice * gpu);
  ~QuadRenderer();

  /// add object to draw cache
  void PushObjectToDraw(const Rect & rect);

  /// submits commands
  void RenderCache(SDL_GPURenderPass * renderPass, SDL_GPUTextureFormat format);

  void UploadToGPU(SDL_GPUCopyPass * copyPass);

private:
  std::vector<Rect> m_rectsToDraw; // CPU cache

  SDL_GPUDevice * m_gpu;

  SDL_GPUGraphicsPipeline * m_pipeline;
  SDL_GPUBuffer * m_gpuData; // GPU cache
  SDL_GPUCommandBuffer * m_commands;

  SDL_GPUShader * m_vertexShader;
  SDL_GPUShader * m_fragmentShader;

};
} // namespace GameFramework
