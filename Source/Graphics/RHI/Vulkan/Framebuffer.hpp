#pragma once
#include <list>

#include <Context.hpp>
#include <vulkan/vulkan.hpp>

#include "VulkanContext.hpp"

namespace RHI::vulkan
{
enum class AttachmentUsage
{
  Color,
  DepthStencil
};

namespace details
{
struct RenderPassBuilder;
struct FramebufferBuilder;
} // namespace details

struct Framebuffer : public IFramebuffer
{
  explicit Framebuffer(const Context & ctx);
  virtual ~Framebuffer() override;
  virtual void Invalidate() override;
  virtual void SetExtent(uint32_t width, uint32_t height) override;
  //virtual void AddColorAttachment(/*ImageFormat*/) override;
  virtual void BeginRendering(CommandBufferHandle cmds) const override;
  virtual void EndRendering(CommandBufferHandle cmds) const override;

  vk::RenderPass GetRenderPass() const noexcept { return m_renderPass; }

protected:
  const Context & m_owner;
  // framebuffer params
  vk::Framebuffer m_framebuffer = VK_NULL_HANDLE;
  vk::RenderPass m_renderPass = VK_NULL_HANDLE;
  vk::Extent2D m_extent;
  vk::ClearValue m_clearValue;
  std::unique_ptr<details::RenderPassBuilder> m_renderPassBuilder;
  std::unique_ptr<details::FramebufferBuilder> m_framebufferBuilder;

  bool m_invalidRenderPass : 1 = true;
  bool m_invalidFramebuffer : 1 = true; ///< invalidity flag

  friend struct DefaultFramebuffer;
};

struct DefaultFramebuffer final : public IFramebuffer
{
  explicit DefaultFramebuffer(const Context & ctx, size_t swapchainSize, VkFormat swapchainFormat,
                              VkSampleCountFlagBits samplesCount);
  virtual ~DefaultFramebuffer() override = default;

  virtual void Invalidate() override;
  virtual void SetExtent(uint32_t width, uint32_t height) override;
  //virtual void AddColorAttachment(/*ImageFormat*/) override;
  virtual void BeginRendering(CommandBufferHandle cmds) const override;
  virtual void EndRendering(CommandBufferHandle cmds) const override;

  void OnSwapBuffers();
  void SetSwapchainImages(const std::vector<VkImageView> & images);

  vk::RenderPass GetRenderPass() const noexcept { return m_current->GetRenderPass(); }

private:
  std::list<Framebuffer> m_framebuffers;
  std::list<Framebuffer>::iterator m_current;
};

} // namespace RHI::vulkan
