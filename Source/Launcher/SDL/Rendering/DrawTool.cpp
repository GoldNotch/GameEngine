#include "DrawTool.hpp"

#include <stdexcept>

#include <Core/Formatter.hpp>
#include <SDL/Rendering/ConnectionGPU.hpp>

namespace GameFramework
{

DrawTool_SDL::DrawTool_SDL(ConnectionGPU & gpu, SDL_Window * wnd)
  : OwnedBy<ConnectionGPU>(gpu)
  , m_window(wnd)
{
  SDL_ClaimWindowForGPUDevice(GetGPU().GetDevice(), m_window);


  m_quadRenderer = std::make_unique<QuadRenderer>(*this);
  m_meshRenderer = std::make_unique<MeshRenderer>(*this);
}


DrawTool_SDL::~DrawTool_SDL()
{
  m_meshRenderer.reset();
  m_quadRenderer.reset();
  // destroy the GPU device
  SDL_DestroyGPUDevice(GetGPU().GetDevice());
}


void DrawTool_SDL::Finish()
{
  m_meshRenderer->UploadToGPU();
  m_quadRenderer->UploadToGPU();
  GetGPU().GetUploader().SubmitAndUpload();

  //acquire the command buffer
  SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(GetGPU().GetDevice());

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

  SDL_GPUColorTargetDescription description{};
  description.format = SDL_GetGPUSwapchainTextureFormat(GetGPU().GetDevice(), m_window);
  m_renderTarget.SetColorAttachment(0, colorTargetInfo, description);

  m_quadRenderer->RenderCache(commandBuffer);
  //m_meshRenderer->RenderCache(commandBuffer);

  // submit the command buffer
  SDL_SubmitGPUCommandBuffer(commandBuffer);
}


void DrawTool_SDL::SetClearColor(const glm::vec4 & color)
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

void DrawTool_SDL::DrawMesh(IStaticMeshResource * mesh)
{
  m_meshRenderer->PushObjectToDraw(mesh);
}

} // namespace GameFramework
