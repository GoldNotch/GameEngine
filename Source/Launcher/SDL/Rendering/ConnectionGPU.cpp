#include "ConnectionGPU.hpp"

#include <stdexcept>

#include <Core/Formatter.hpp>

namespace GameFramework
{

ConnectionGPU::ConnectionGPU()
{
  m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_MSL, true, NULL);
  if (!m_device)
    throw std::runtime_error("Failed to connect to GPU");
  m_uploader = std::make_unique<Uploader>(*this);
}

ConnectionGPU::~ConnectionGPU()
{
  m_uploader.reset();
  // destroy the GPU device
  SDL_DestroyGPUDevice(m_device);
}

std::unique_ptr<DrawTool_SDL> ConnectionGPU::CreateDrawTool() const
{
  return nullptr; //std::make_unique<DrawTool_SDL>(*this);
}

SDL_GPUShader * ConnectionGPU::BuildSpirVShader(const std::filesystem::path & path,
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
  SDL_GPUShader * result = SDL_CreateGPUShader(m_device, &info);
  if (!code)
  {
    throw std::runtime_error(Formatter() << "Failed to build a shader - " << SDL_GetError());
  }
  // free the file
  SDL_free(code);
  return result;
}

ConnectionGPU & GpuConnection()
{
  static ConnectionGPU conn;
  return conn;
}

} // namespace GameFramework
