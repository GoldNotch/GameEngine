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


  SDL_GPUTextureFormat format = SDL_GetGPUSwapchainTextureFormat(m_gpu, m_window);
  m_uploader = std::make_unique<Uploader>(*this);
  m_quadRenderer = std::make_unique<QuadRenderer>(*this, format);
}


DrawTool_SDL::~DrawTool_SDL()
{
  m_quadRenderer.reset();
  m_uploader.reset();
  // destroy the GPU device
  SDL_DestroyGPUDevice(m_gpu);
}

void DrawTool_SDL::Flush()
{
}


void DrawTool_SDL::Finish()
{
  m_uploader->SubmitAndUpload();

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

  SDL_GPURenderPass * renderPass = SDL_BeginGPURenderPass(commandBuffer, &colorTargetInfo, 1, NULL);
  m_quadRenderer->RenderCache(renderPass);
  SDL_EndGPURenderPass(renderPass);

  // submit the command buffer
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}

SDL_GPUDevice * DrawTool_SDL::GetDevice() const noexcept
{
  return m_gpu;
}

Uploader & DrawTool_SDL::GetUploader() & noexcept
{
  return *m_uploader;
}

void DrawTool_SDL::SetClearColor(const std::array<float, 4> & color)
{
  m_clearColor = color;
}

/*
* left, top, right, bottom - in NDC
*/
void DrawTool_SDL::DrawRect(float left, float top, float right, float bottom)
{
  m_quadRenderer->PushObjectToDraw(Rect(left, top, right, bottom));
}

} // namespace GameFramework
