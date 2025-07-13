#include "DrawTool.hpp"

#include <stdexcept>

namespace GameFramework
{

DrawTool_SDL::DrawTool_SDL(SDL_Window * wnd)
  : m_window(wnd)
{
  m_gpu = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  if (!m_gpu)
    throw std::runtime_error("Failed to connect to GPU");

  SDL_ClaimWindowForGPUDevice(m_gpu, m_window);
}


DrawTool_SDL::~DrawTool_SDL()
{
  // destroy the GPU device
  SDL_DestroyGPUDevice(m_gpu);
}

void DrawTool_SDL::Flush()
{
}


void DrawTool_SDL::Finish()
{
  //acquire the command buffer
  SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(m_gpu);

  // get the swapchain texture
  SDL_GPUTexture * swapchainTexture;
  Uint32 width, height;
  SDL_WaitAndAcquireGPUSwapchainTexture(commandBuffer, m_window, &swapchainTexture, &width,
                                        &height);

  // end the frame early if a swapchain texture is not available
  if (swapchainTexture == NULL)
  {
    // you must always submit the command buffer
    SDL_SubmitGPUCommandBuffer(commandBuffer);
    return;
  }

  // create the color target
  SDL_GPUColorTargetInfo colorTargetInfo{};
  colorTargetInfo.clear_color = {m_clearColor[0], m_clearColor[1], m_clearColor[2],
                                 m_clearColor[3]};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapchainTexture;

  // begin a render pass
  SDL_GPURenderPass * renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);

  // draw something


  // end the render pass
  SDL_EndGPURenderPass(renderPass);

  // submit the command buffer
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}
void DrawTool_SDL::SetClearColor(const std::array<float, 4> & color)
{
  m_clearColor = color;
}
} // namespace GameFramework
