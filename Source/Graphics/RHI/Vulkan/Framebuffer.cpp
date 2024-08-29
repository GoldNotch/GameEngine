#include "Framebuffer.hpp"

#include <Formatter.hpp>

#include "Utils/Builders.hpp"

namespace RHI::vulkan
{

Framebuffer::Framebuffer(const Context & ctx)
  : m_owner(ctx)
  , m_renderPassBuilder(new details::RenderPassBuilder())
  , m_framebufferBuilder(new details::FramebufferBuilder())
{
}

Framebuffer::~Framebuffer()
{
  if (m_framebuffer != VK_NULL_HANDLE)
    vkDestroyFramebuffer(m_owner.GetDevice(), m_framebuffer, nullptr);
  if (m_renderPass != VK_NULL_HANDLE)
    vkDestroyRenderPass(m_owner.GetDevice(), m_renderPass, nullptr);
}

void Framebuffer::Invalidate()
{
  if (m_invalidRenderPass || m_renderPass == VK_NULL_HANDLE)
  {
    auto new_renderpass = m_renderPassBuilder->Make(m_owner.GetDevice());
    if (m_renderPass != VK_NULL_HANDLE)
      vkDestroyRenderPass(m_owner.GetDevice(), m_renderPass, nullptr);
    m_renderPass = new_renderpass;
  }

  if (m_invalidFramebuffer || m_framebuffer == VK_NULL_HANDLE)
  {
    auto new_framebuffer = m_framebufferBuilder->Make(m_owner.GetDevice());
    if (m_framebuffer != VK_NULL_HANDLE)
      vkDestroyFramebuffer(m_owner.GetDevice(), m_framebuffer, nullptr);
    m_framebuffer = new_framebuffer;
  }

  m_invalidRenderPass = m_invalidFramebuffer = false;
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
  renderPassInfo.framebuffer = m_framebuffer;
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

// --------------------- Default Framebuffer ------------------------

DefaultFramebuffer::DefaultFramebuffer(const Context & ctx, size_t swapchainSize,
                                       VkFormat swapchainFormat, VkSampleCountFlagBits samplesCount)
{
  if (swapchainSize == 0)
    throw std::runtime_error("Empty swapchain");

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

  for (size_t i = 0; i < swapchainSize; ++i)
  {
    auto && fbo = m_framebuffers.emplace_back(ctx);
    fbo.m_framebufferBuilder->AddAttachment(VK_NULL_HANDLE);
    fbo.m_renderPassBuilder->AddAttachment(swapchainAttachment);
  }
  m_current = m_framebuffers.begin();
}

void DefaultFramebuffer::Invalidate()
{
  for (auto && framebuffer : m_framebuffers)
    framebuffer.Invalidate();
  m_current = m_framebuffers.begin();
}

void DefaultFramebuffer::SetExtent(uint32_t width, uint32_t height)
{
  for (auto && framebuffer : m_framebuffers)
    framebuffer.SetExtent(width, height);
}

void DefaultFramebuffer::BeginRendering(CommandBufferHandle cmds) const
{
  m_current->BeginRendering(cmds);
}

void DefaultFramebuffer::EndRendering(CommandBufferHandle cmds) const
{
  m_current->EndRendering(cmds);
}

void DefaultFramebuffer::OnSwapBuffers()
{
  m_current = std::next(m_current);
  if (m_current == m_framebuffers.end())
    m_current = m_framebuffers.begin();
}

void DefaultFramebuffer::SetSwapchainImages(const std::vector<VkImageView> & images)
{
  assert(images.size() == m_framebuffers.size());
  auto it = m_framebuffers.begin();
  auto img_it = images.begin();
  while (it != m_framebuffers.end() && img_it != images.end())
  {
    it->m_framebufferBuilder->SetAttachment(0) = *img_it;
    it->m_invalidFramebuffer = true;
    img_it = std::next(img_it);
    it = std::next(it);
  }
}

} // namespace RHI::vulkan
