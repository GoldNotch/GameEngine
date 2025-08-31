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

inline bool operator==(const SDL_GPUGraphicsPipelineTargetInfo & l,
                       const SDL_GPUGraphicsPipelineTargetInfo & r) noexcept
{
  return l.num_color_targets == r.num_color_targets &&
         l.depth_stencil_format == r.depth_stencil_format &&
         l.has_depth_stencil_target == r.has_depth_stencil_target &&
         std::memcmp(l.color_target_descriptions, r.color_target_descriptions,
                     l.num_color_targets * sizeof(SDL_GPUColorTargetDescription));
}
