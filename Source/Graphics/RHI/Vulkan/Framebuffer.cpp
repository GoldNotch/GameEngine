#include "Framebuffer.hpp"

#include <Formatter.hpp>

#include "Utils/Builders.hpp"

namespace RHI::vulkan
{

Framebuffer::Framebuffer(const Context & ctx, uint32_t buffersCount)
  : m_owner(ctx)
  , m_renderPassBuilder(new details::RenderPassBuilder())
  , m_buffersCount(buffersCount)
{
  if (m_buffersCount == 0)
    throw std::runtime_error("No buffers to create");

  for (uint32_t i = 0; i < m_buffersCount; ++i)
    m_framebuffers.push_back({VK_NULL_HANDLE, details::FramebufferBuilder()});
  m_current = m_framebuffers.begin();
}

Framebuffer::~Framebuffer()
{
  for (auto && [framebuffer, builder] : m_framebuffers)
    if (!!framebuffer)
      vkDestroyFramebuffer(m_owner.GetDevice(), framebuffer, nullptr);
  if (!!m_renderPass)
    vkDestroyRenderPass(m_owner.GetDevice(), m_renderPass, nullptr);
}

void Framebuffer::Invalidate()
{
  if (m_invalidRenderPass || !m_renderPass)
  {
    auto new_renderpass = m_renderPassBuilder->Make(m_owner.GetDevice());
    if (!!m_renderPass)
      vkDestroyRenderPass(m_owner.GetDevice(), m_renderPass, nullptr);
    m_renderPass = new_renderpass;
  }

  InvalidateFramebuffers();
  m_invalidRenderPass = false;
}

void Framebuffer::SetExtent(uint32_t width, uint32_t height)
{
  m_extent = VkExtent2D{width, height};
  m_invalidFramebuffer = true;
}

void Framebuffer::BeginRendering(CommandBufferHandle cmds) const
{
  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = m_renderPass;
  renderPassInfo.framebuffer = m_current->first;
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = m_extent;
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = reinterpret_cast<const VkClearValue *>(&m_clearValue);

  vkCmdBeginRenderPass(reinterpret_cast<VkCommandBuffer>(cmds), &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
}

void Framebuffer::EndRendering(CommandBufferHandle cmds) const
{
  vkCmdEndRenderPass(reinterpret_cast<VkCommandBuffer>(cmds));
}

RenderPassHandle Framebuffer::GetRenderPass() const noexcept
{
  return static_cast<VkRenderPass>(m_renderPass);
}

void Framebuffer::OnSwapBuffers()
{
  m_current = std::next(m_current);
  if (m_current == m_framebuffers.end())
    m_current = m_framebuffers.begin();
}

void Framebuffer::InvalidateFramebuffers()
{
  for (auto && [framebuffer, builder] : m_framebuffers)
  {
    if (m_invalidRenderPass || m_invalidFramebuffer || !framebuffer)
    {
      auto new_framebuffer = builder.Make(m_owner.GetDevice());
      if (!!framebuffer)
        vkDestroyFramebuffer(m_owner.GetDevice(), framebuffer, nullptr);
      framebuffer = new_framebuffer;
    }
  }
  //m_current = m_framebuffers.begin();
  m_invalidFramebuffer = false;
}

// --------------------- Default Framebuffer ------------------------

DefaultFramebuffer::DefaultFramebuffer(const Context & ctx, uint32_t buffersCount,
                                       VkFormat swapchainFormat, VkSampleCountFlagBits samplesCount)
  : Framebuffer(ctx, buffersCount)
{
  VkAttachmentDescription swapchainAttachment{};
  swapchainAttachment.format = swapchainFormat;
  swapchainAttachment.samples = samplesCount; // MSAA
  swapchainAttachment.loadOp =
    VK_ATTACHMENT_LOAD_OP_CLEAR; // action for color/depth buffers in pass begins
  swapchainAttachment.storeOp =
    VK_ATTACHMENT_STORE_OP_STORE; // action for color/depth buffers in pass ends
  swapchainAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // action
  swapchainAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  swapchainAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  swapchainAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

  m_renderPassBuilder->AddAttachment(swapchainAttachment);
  for (auto && [framebuffer, builder] : m_framebuffers)
    builder.AddAttachment(VK_NULL_HANDLE);
}

void DefaultFramebuffer::SetSwapchainImages(const std::vector<VkImageView> & images)
{
  assert(images.size() == m_framebuffers.size());
  auto it = m_framebuffers.begin();
  auto img_it = images.begin();
  while (it != m_framebuffers.end() && img_it != images.end())
  {
    it->second.SetAttachment(0) = *img_it;
    it = std::next(it);
    img_it = std::next(img_it);
  }
  m_invalidFramebuffer = true;
}

} // namespace RHI::vulkan
