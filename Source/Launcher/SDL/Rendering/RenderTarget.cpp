#include "RenderTarget.hpp"

#include <SDL/Rendering/ConnectionGPU.hpp>
#include <SDL/Rendering/DrawTool.hpp>

namespace GameFramework
{

RenderTarget::RenderTarget(SDL_GPUDevice * device)
{
  //acquire the command buffer
  m_commandBuffer = SDL_AcquireGPUCommandBuffer(device);
}

RenderTarget::RenderTarget(RenderTarget && rhs) noexcept
{
  std::swap(m_commandBuffer, rhs.m_commandBuffer);
  std::swap(m_colorTargetsInfo, rhs.m_colorTargetsInfo);
  std::swap(m_colorTargetsDescription, rhs.m_colorTargetsDescription);
  std::swap(m_depthStencilFormat, rhs.m_depthStencilFormat);
  std::swap(m_depthStencilTargetInfo, rhs.m_depthStencilTargetInfo);
}

RenderTarget & RenderTarget::operator=(RenderTarget && rhs) noexcept
{
  if (this != &rhs)
  {
    std::swap(m_commandBuffer, rhs.m_commandBuffer);
    std::swap(m_colorTargetsInfo, rhs.m_colorTargetsInfo);
    std::swap(m_colorTargetsDescription, rhs.m_colorTargetsDescription);
    std::swap(m_depthStencilFormat, rhs.m_depthStencilFormat);
    std::swap(m_depthStencilTargetInfo, rhs.m_depthStencilTargetInfo);
  }
  return *this;
}

RenderTarget::~RenderTarget()
{
  // you must always submit the command buffer
  SDL_SubmitGPUCommandBuffer(m_commandBuffer);
}

void RenderTarget::SetColorAttachment(uint32_t index, const SDL_GPUColorTargetInfo & info,
                                      SDL_GPUColorTargetDescription & description)
{
  while (m_colorTargetsInfo.size() <= index)
    m_colorTargetsInfo.emplace_back();
  while (m_colorTargetsDescription.size() <= index)
    m_colorTargetsDescription.emplace_back();

  m_colorTargetsInfo[index] = info;
  m_colorTargetsDescription[index] = description;
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
  SDL_GPURenderPass * renderPass =
    SDL_BeginGPURenderPass(cmdBuf, m_colorTargetsInfo.data(), m_colorTargetsInfo.size(),
                           m_depthStencilTargetInfo.has_value() ? &m_depthStencilTargetInfo.value()
                                                                : nullptr);
  return std::unique_ptr<SDL_GPURenderPass, RenderPassDeleter>(renderPass, SDL_EndGPURenderPass);
}

} // namespace GameFramework
