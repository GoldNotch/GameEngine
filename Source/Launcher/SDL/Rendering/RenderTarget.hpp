#pragma once
#include <memory>
#include <optional>
#include <span>
#include <vector>

#include <OwnedBy.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct WindowDC_SDL;

struct RenderTarget final
{
  RenderTarget() = default;
  explicit RenderTarget(SDL_GPUDevice * device);
  RenderTarget(RenderTarget && rhs) noexcept;
  RenderTarget & operator=(RenderTarget && rhs) noexcept;
  RenderTarget(const RenderTarget &) = delete;
  RenderTarget & operator=(const RenderTarget &) = delete;
  ~RenderTarget();

  void SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                          SDL_GPUColorTargetDescription & description);
  void SetDepthStencilAttachment(
    const SDL_GPUDepthStencilTargetInfo * info,
    SDL_GPUTextureFormat textureFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID);

  /// Get TargetsInfo to build pipeline
  SDL_GPUGraphicsPipelineTargetInfo GetTargetsInfo() const;
  SDL_GPUCommandBuffer * GetCommandBuffer() noexcept { return m_commandBuffer; }

  typedef void (*RenderPassDeleter)(SDL_GPURenderPass * rp);
  /// create RAII RenderPass
  std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter> CreateRenderPass(
    SDL_GPUCommandBuffer * cmdBuf) const;

private:
  SDL_GPUCommandBuffer * m_commandBuffer = nullptr;
  std::vector<SDL_GPUColorTargetInfo> m_colorTargetsInfo; ///< info to begin RenderPass
  std::vector<SDL_GPUColorTargetDescription> m_colorTargetsDescription; ///< info to build pipeline
  std::optional<SDL_GPUDepthStencilTargetInfo>
    m_depthStencilTargetInfo; ///< info to begin RenderPass
  SDL_GPUTextureFormat m_depthStencilFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
};

} // namespace GameFramework
