#pragma once
#include <filesystem>

#include <SDL3/SDL.h>

#include "DrawTool.hpp"
#include "Uploader.hpp"

namespace GameFramework
{

struct ConnectionGPU final
{
  ConnectionGPU();
  ~ConnectionGPU();

  SDL_GPUDevice * GetDevice() const noexcept { return m_device; }
  Uploader & GetUploader() & noexcept { return *m_uploader; }
  std::unique_ptr<DrawTool_SDL> CreateDrawTool() const;

  SDL_GPUShader * BuildSpirVShader(const std::filesystem::path & path, SDL_GPUShaderStage stage,
                                   uint32_t numSamplers = 0, uint32_t numUniforms = 0,
                                   uint32_t numSSBO = 0, uint32_t numSSTO = 0) const;

private:
  SDL_GPUDevice * m_device = nullptr;
  std::unique_ptr<Uploader> m_uploader;
};

ConnectionGPU & GpuConnection();

} // namespace GameFramework
