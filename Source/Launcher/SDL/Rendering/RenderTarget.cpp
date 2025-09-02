#include "RenderTarget.hpp"

#include <SDL/Rendering/ConnectionGPU.hpp>
#include <SDL/Rendering/DrawTool.hpp>

namespace GameFramework
{

void RenderTargetInfo::SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                                          SDL_GPUColorTargetDescription & description)
{
  while (m_colorTargetsInfo.size() <= index)
    m_colorTargetsInfo.emplace_back();
  while (m_colorTargetsDescription.size() <= index)
    m_colorTargetsDescription.emplace_back();

  m_colorTargetsInfo[index] = info;
  if (description != m_colorTargetsDescription[index])
  {
    m_colorTargetsDescription[index] = description;
    m_shouldRebuildPipelines = true;
  }
}

void RenderTargetInfo::SetDepthStencilAttachment(const SDL_GPUDepthStencilTargetInfo * info,
                                                 SDL_GPUTextureFormat textureFormat)
{
  SDL_GPUTextureFormat newFormat;
  if (info)
  {
    m_depthStencilTargetInfo.emplace(*info);
    newFormat = textureFormat;
  }
  else
  {
    m_depthStencilTargetInfo.reset();
    newFormat = SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
  }

  if (m_depthStencilFormat != newFormat)
  {
    m_depthStencilFormat = newFormat;
    m_shouldRebuildPipelines = true;
  }
}


SDL_GPUGraphicsPipelineTargetInfo RenderTargetInfo::GetTargetsInfo() const
{
  SDL_GPUGraphicsPipelineTargetInfo targetInfo{};
  targetInfo.num_color_targets = m_colorTargetsDescription.size();
  targetInfo.color_target_descriptions = m_colorTargetsDescription.data();
  targetInfo.depth_stencil_format = m_depthStencilFormat;
  targetInfo.has_depth_stencil_target = m_depthStencilFormat !=
                                        SDL_GPUTextureFormat::SDL_GPU_TEXTUREFORMAT_INVALID;
  return targetInfo;
}

RenderTarget::RenderTarget(SDL_GPUDevice * device)
  : m_device(device)
{
}

RenderTarget::RenderTarget(RenderTarget && rhs) noexcept
{
  std::swap(m_commandBuffer, rhs.m_commandBuffer);
  std::swap(m_device, rhs.m_device);
  std::swap(m_info, rhs.m_info);
}

RenderTarget & RenderTarget::operator=(RenderTarget && rhs) noexcept
{
  if (this != &rhs)
  {
    std::swap(m_commandBuffer, rhs.m_commandBuffer);
    std::swap(m_device, rhs.m_device);
    std::swap(m_info, rhs.m_info);
  }
  return *this;
}

bool RenderTarget::BeginPass()
{
  m_commandBuffer = SDL_AcquireGPUCommandBuffer(m_device);
  return m_commandBuffer != nullptr;
}

void RenderTarget::EndPass()
{
  SDL_SubmitGPUCommandBuffer(m_commandBuffer);
}

RenderTarget::RaiiRenderPass RenderTarget::CreateSubPass() const
{
  SDL_GPURenderPass * renderPass =
    SDL_BeginGPURenderPass(m_commandBuffer, m_info.m_colorTargetsInfo.data(),
                           m_info.m_colorTargetsInfo.size(),
                           m_info.m_depthStencilTargetInfo.has_value()
                             ? &m_info.m_depthStencilTargetInfo.value()
                             : nullptr);
  return std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter>(renderPass, SDL_EndGPURenderPass);
}

} // namespace GameFramework
