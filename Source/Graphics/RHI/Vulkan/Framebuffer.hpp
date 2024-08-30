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
  virtual RenderPassHandle GetRenderPass() const noexcept override;
  virtual void OnSwapBuffers() override;

protected:
  using BuildedFramebuffer = std::pair<vk::Framebuffer, details::FramebufferBuilder>;

  const Context & m_owner;
  uint32_t m_buffersCount; ///< m_framebuffers count (aka double buffering, triple buffering, etc)
  std::list<BuildedFramebuffer> m_framebuffers;
  std::list<BuildedFramebuffer>::iterator m_current;
  vk::RenderPass m_renderPass = VK_NULL_HANDLE;
  vk::Extent2D m_extent;
  vk::ClearValue m_clearValue;
  std::unique_ptr<details::RenderPassBuilder> m_renderPassBuilder;

  bool m_invalidRenderPass : 1 = true;
  bool m_invalidFramebuffer : 1 = true; ///< invalidity flag

  friend struct DefaultFramebuffer;

private:
  void InvalidateFramebuffers();
};

struct DefaultFramebuffer final : public Framebuffer
{
  explicit DefaultFramebuffer(const Context & ctx, VkFormat swapchainFormat,
                              VkSampleCountFlagBits samplesCount);

  void SetSwapchainImages(const std::vector<VkImageView> & images);
};

} // namespace RHI::vulkan
