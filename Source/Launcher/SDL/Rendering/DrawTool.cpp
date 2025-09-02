#include "DrawTool.hpp"

#include <stdexcept>

#include <Core/Formatter.hpp>
#include <SDL/Rendering/ConnectionGPU.hpp>

namespace GameFramework
{

DrawTool_SDL::DrawTool_SDL(ConnectionGPU & gpu)
  : OwnedBy<ConnectionGPU>(gpu)
{
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

  if (m_renderTarget)
  {
    if (m_renderTarget->GetInfo().ShouldRebuildPipelines())
    {
      m_quadRenderer->RebuildPipeline(*m_renderTarget);
      m_meshRenderer->RebuildPipeline(*m_renderTarget);
      m_renderTarget->GetInfo().NotifyPipelinesRebuilt();
    }

    m_quadRenderer->RenderCache(*m_renderTarget);
    //m_meshRenderer->RenderCache(*m_renderTarget);
  }
}


const RenderTarget * DrawTool_SDL::ExchangeRenderTarget(const RenderTarget * rt) noexcept
{
  return std::exchange(m_renderTarget, rt);
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

WindowDC_SDL::WindowDC_SDL(DrawTool_SDL & drawTool, SDL_Window * window)
  : OwnedBy<DrawTool_SDL>(drawTool)
  , m_target(drawTool.GetGPU().GetDevice())
  , m_window(window)
{
  SDL_ClaimWindowForGPUDevice(GetDrawTool().GetGPU().GetDevice(), m_window);
}

WindowDC_SDL::~WindowDC_SDL() = default;

bool WindowDC_SDL::AcquireBuffer()
{
  if (!m_target.BeginPass())
    return false;

  // get the swapchain texture
  SDL_GPUTexture * swapchainTexture;
  Uint32 width, height;
  SDL_WaitAndAcquireGPUSwapchainTexture(m_target.GetCommandBuffer(), m_window, &swapchainTexture,
                                        &width, &height);

  // end the frame early if a swapchain texture is not available
  if (swapchainTexture == NULL)
  {
    m_target.EndPass();
    return false;
  }

  // create the color target
  SDL_GPUColorTargetInfo colorTargetInfo{};
  auto clearColor = GetDrawTool().GetClearColor();
  colorTargetInfo.clear_color = {clearColor.r, clearColor.g, clearColor.b, clearColor.a};
  colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
  colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;
  colorTargetInfo.texture = swapchainTexture;

  SDL_GPUColorTargetDescription description{};
  description.format =
    SDL_GetGPUSwapchainTextureFormat(GetDrawTool().GetGPU().GetDevice(), m_window);
  m_target.GetInfo().SetColorAttachment(0, colorTargetInfo, description);
  m_oldRt = GetDrawTool().ExchangeRenderTarget(&m_target);
}

void WindowDC_SDL::SubmitBuffer()
{
  auto * curDC = GetDrawTool().ExchangeRenderTarget(m_oldRt);
  assert(curDC == &m_target);
  m_target.EndPass();
}

} // namespace GameFramework
