#include <Formatter.hpp>
#include <vulkan/vulkan.hpp>

#include "Builders.hpp"

namespace RHI::vulkan::details //-------- RenderPass Builder -------------
{
void RenderPassBuilder::AddAttachment(const VkAttachmentDescription & description)
{
  m_attachments.push_back(description);
}

vk::RenderPass RenderPassBuilder::Make(const vk::Device & device) const
{
  VkRenderPass renderPass{};

  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(m_attachments.size());
  renderPassCreateInfo.pAttachments = m_attachments.data();
  renderPassCreateInfo.subpassCount = static_cast<uint32_t>(m_subpasses.size());
  renderPassCreateInfo.pSubpasses = m_subpasses.data();
  renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
  renderPassCreateInfo.pDependencies = m_dependencies.data();


  if (auto res = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass);
      res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create render pass - " << res);

  return vk::RenderPass(renderPass);
}

void RenderPassBuilder::Reset()
{
  m_attachmentRefs.clear();
  m_attachments.clear();
  m_dependencies.clear();
  m_subpasses.clear();
  //m_usageFlags.clear();
}

} // namespace RHI::vulkan::details


namespace RHI::vulkan::details //--------------- Framebuffer builder ------------
{
void FramebufferBuilder::SetRenderPass(const vk::RenderPass & renderPass)
{
  m_renderPass = renderPass;
}

void FramebufferBuilder::SetExtent(uint32_t width, uint32_t height)
{
  m_extent.width = width;
  m_extent.height = height;
}

void FramebufferBuilder::AddAttachment(const vk::ImageView & image)
{
  m_images.push_back(image);
}

vk::ImageView & FramebufferBuilder::SetAttachment(size_t idx) & noexcept
{
  return m_images[idx];
}

vk::Framebuffer FramebufferBuilder::Make(const vk::Device & device) const
{
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = m_renderPass;
  framebufferInfo.attachmentCount = static_cast<uint32_t>(m_images.size());
  framebufferInfo.pAttachments = reinterpret_cast<const VkImageView *>(m_images.data());
  framebufferInfo.width = m_extent.width;
  framebufferInfo.height = m_extent.height;
  framebufferInfo.layers = 1; // кол-во изображений
  VkFramebuffer framebuffer;
  if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer) != VK_SUCCESS)
    throw std::runtime_error("failed to create framebuffer!");
  return vk::Framebuffer(framebuffer);
}

void FramebufferBuilder::Reset()
{
  m_images.clear();
  m_extent = vk::Extent2D{0, 0};
}

} // namespace RHI::vulkan::details
