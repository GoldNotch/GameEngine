#include "RenderTarget.hpp"

namespace GameFramework
{
void RenderTarget::SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                                      SDL_GPUColorTargetDescription & description)
{
  while (m_colorTargetsInfo.size() <= index)
    m_colorTargetsInfo.emplace_back();
  while (m_colorTargetsDescription.size() <= index)
    m_colorTargetsDescription.emplace_back();

  m_colorTargetsInfo[index] = info;
  m_colorTargetsDescription[index] = description;
  m_changed = true;
}

void RenderTarget::SetDepthStencilAttachment(const SDL_GPUDepthStencilTargetInfo * info,
                                             SDL_GPUTextureFormat textureFormat)
{
  if (info)
  {
    m_depthStencilTargetInfo.emplace(*info);
    m_depthStencilFormat = textureFormat;
  }
  else
  {
    m_depthStencilTargetInfo.reset();
    m_depthStencilFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
  }
  m_changed = true;
}


SDL_GPUGraphicsPipelineTargetInfo RenderTarget::GetTargetsInfo() const
{
  SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
  targetInfo.num_color_targets = m_colorTargetsDescription.size();
  targetInfo.color_target_descriptions = m_colorTargetsDescription.data();
  targetInfo.depth_stencil_format = m_depthStencilFormat;
  targetInfo.has_depth_stencil_target = m_depthStencilTargetInfo.has_value();
  return targetInfo;
}

std::unique_ptr<SDL_GPURenderPass, RenderTarget::RenderPassDeleter> RenderTarget::CreateRenderPass(
  SDL_GPUCommandBuffer * cmdBuf) const
{
  m_changed = false;
  SDL_GPURenderPass * renderPass =
    SDL_BeginGPURenderPass(cmdBuf, m_colorTargetsInfo.data(), m_colorTargetsInfo.size(),
                           m_depthStencilTargetInfo.has_value() ? &m_depthStencilTargetInfo.value()
                                                                : nullptr);
  return std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter>(renderPass, SDL_EndGPURenderPass);
}

bool RenderTarget::ShouldRebuildPipelines() const
{
  return m_changed;
}

} // namespace GameFramework
