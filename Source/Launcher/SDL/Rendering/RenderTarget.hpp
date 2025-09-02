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

struct RenderTargetInfo final
{
  RenderTargetInfo() = default;
  friend struct RenderTarget;

  void SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                          SDL_GPUColorTargetDescription & description);
  void SetDepthStencilAttachment(
    const SDL_GPUDepthStencilTargetInfo * info,
    SDL_GPUTextureFormat textureFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID);

  /// Get TargetsInfo to build pipeline
  SDL_GPUGraphicsPipelineTargetInfo GetTargetsInfo() const;

  bool ShouldRebuildPipelines() const noexcept { return m_shouldRebuildPipelines; }
  void NotifyPipelinesRebuilt() const noexcept { m_shouldRebuildPipelines = false; }

private:
  mutable bool m_shouldRebuildPipelines = false;
  std::vector<SDL_GPUColorTargetInfo> m_colorTargetsInfo; ///< info to begin RenderPass
  std::vector<SDL_GPUColorTargetDescription> m_colorTargetsDescription; ///< info to build pipeline
  std::optional<SDL_GPUDepthStencilTargetInfo>
    m_depthStencilTargetInfo; ///< info to begin RenderPass
  SDL_GPUTextureFormat m_depthStencilFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
};

struct RenderTarget final
{
  explicit RenderTarget(SDL_GPUDevice * device);
  RenderTarget(RenderTarget && rhs) noexcept;
  RenderTarget & operator=(RenderTarget && rhs) noexcept;
  ~RenderTarget() = default;

  bool BeginPass();
  void EndPass();
  SDL_GPUCommandBuffer * GetCommandBuffer() noexcept { return m_commandBuffer; }
  RenderTargetInfo & GetInfo() & noexcept { return m_info; }
  const RenderTargetInfo & GetInfo() const & noexcept { return m_info; }

  typedef void (*RenderPassDeleter)(SDL_GPURenderPass * rp);
  using RaiiRenderPass = std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter>;
  /// create RAII RenderPass
  RaiiRenderPass CreateSubPass() const;

private:
  SDL_GPUDevice * m_device = nullptr;
  SDL_GPUCommandBuffer * m_commandBuffer = nullptr;
  RenderTargetInfo m_info;
};

} // namespace GameFramework
