#include "DrawTool.hpp"

#include <stdexcept>

#include <Core/Formatter.hpp>

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
  m_meshRenderer = std::make_unique<MeshRenderer>(*this, format);
}


DrawTool_SDL::~DrawTool_SDL()
{
  m_meshRenderer.reset();
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
  m_meshRenderer->UploadToGPU();
  m_quadRenderer->UploadToGPU();
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
  //m_meshRenderer->RenderCache(renderPass);
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

SDL_GPUShader * DrawTool_SDL::BuildSpirVShader(const std::filesystem::path & path,
                                               SDL_GPUShaderStage stage, uint32_t numSamplers,
                                               uint32_t numUniforms, uint32_t numSSBO,
                                               uint32_t numSSTO) const
{
  // load the vertex shader code
  size_t codeSize = 0;
  std::filesystem::path shaderPath{".shaders"};
  shaderPath /= path;
  if (shaderPath.extension() == ".vert")
    shaderPath.replace_extension("_vert.spv");
  if (shaderPath.extension() == ".frag")
    shaderPath.replace_extension("_frag.spv");
  if (shaderPath.extension() == ".geom")
    shaderPath.replace_extension("_geom.spv");

  void * code = SDL_LoadFile(shaderPath.string().c_str(), &codeSize);
  if (!code)
  {
    throw std::runtime_error(Formatter() << "Failed to load a shader file - " << SDL_GetError());
  }
  // create the vertex shader
  SDL_GPUShaderCreateInfo info{};
  info.code = (Uint8 *)code; //convert to an array of bytes
  info.code_size = codeSize;
  info.entrypoint = "main";
  info.format = SDL_GPU_SHADERFORMAT_SPIRV; // loading .spv shaders
  info.stage = stage;
  info.num_samplers = numSamplers;
  info.num_storage_buffers = numSSBO;
  info.num_storage_textures = numSSTO;
  info.num_uniform_buffers = numUniforms;
  SDL_GPUShader * result = SDL_CreateGPUShader(m_gpu, &info);
  if (!code)
  {
    throw std::runtime_error(Formatter() << "Failed to build a shader - " << SDL_GetError());
  }
  // free the file
  SDL_free(code);
  return result;
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

void DrawTool_SDL::DrawMesh(IStaticMeshResouce * mesh)
{
  m_meshRenderer->PushObjectToDraw(mesh);
}

} // namespace GameFramework
