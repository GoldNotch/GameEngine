#pragma once
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <SDL3/SDL.h>

namespace GameFramework
{

struct RenderTarget final
{
  RenderTarget() = default;
  ~RenderTarget() = default;

  void SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                          SDL_GPUColorTargetDescription & description);
  void SetDepthStencilAttachment(const SDL_GPUDepthStencilTargetInfo * info,
    SDL_GPUTextureFormat textureFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID);

  /// Get TargetsInfo to build pipeline
  SDL_GPUGraphicsPipelineTargetInfo GetTargetsInfo() const;

  typedef void (*RenderPassDeleter)(SDL_GPURenderPass * rp);
  /// create RAII RenderPass
  std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter> CreateRenderPass(
    SDL_GPUCommandBuffer * cmdBuf) const;

  bool ShouldRebuildPipelines() const;

private:
  mutable bool m_changed = false;
  std::vector<SDL_GPUColorTargetInfo> m_colorTargetsInfo; ///< info to begin RenderPass
  std::vector<SDL_GPUColorTargetDescription> m_colorTargetsDescription; ///< info to build pipeline
  std::optional<SDL_GPUDepthStencilTargetInfo>
    m_depthStencilTargetInfo; ///< info to begin RenderPass
  SDL_GPUTextureFormat m_depthStencilFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
};

} // namespace GameFramework
