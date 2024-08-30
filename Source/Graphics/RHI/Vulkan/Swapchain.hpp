#pragma once
#include <list>
#include <vector>

#include "Framebuffer.hpp"
#include "VulkanContext.hpp"

namespace vkb
{
struct Swapchain;
}

namespace RHI::vulkan
{

struct FrameData final : public IFrameData
{
  explicit FrameData(const Context & ctx, VkFormat swapchainFormat);
  virtual ~FrameData() override;

  virtual void WaitForRenderingComplete() const override;
  virtual void Submit(std::vector<CommandBufferHandle> && buffers) const override;

  const VkSemaphore & GetImageAvailableSemaphore() const & { return image_available_semaphore; }
  const VkSemaphore & GetRenderingFinishedSemaphore() const & { return render_finished_semaphore; }
  const VkFence & GetRenderingFence() const & { return is_rendering; }

private:
  const Context & m_owner; ///< context, doesn't own
  VkQueue m_queue;
  uint32_t m_queueIndex;

  // owned data
  VkSemaphore image_available_semaphore;
  VkSemaphore render_finished_semaphore;
  VkFence is_rendering;

private:
  FrameData(const FrameData &) = delete;
  FrameData & operator=(const FrameData &) = delete;
};


/// @brief vulkan implementation for renderer
struct Swapchain final : public ISwapchain
{
  explicit Swapchain(const Context & ctx, const vk::SurfaceKHR surface);
  virtual ~Swapchain() override;

  /// @brief destroys old surface data like framebuffers, images, images_views, ets and creates new
  virtual void Invalidate() override;
  //virtual IFrameData & AcquireFrame() & override;
  virtual void SwapBuffers() override;
  virtual const IFramebuffer & GetDefaultFramebuffer() const & noexcept override;
  virtual uint32_t GetBuffersCount() const override;
  
private:
  const Context & m_owner;

  vk::Queue m_renderQueue = VK_NULL_HANDLE;
  uint32_t m_renderFamilyIndex;
  vk::Queue m_presentQueue = VK_NULL_HANDLE;
  uint32_t m_presentFamilyIndex;

  /// presentaqtion data
  vk::SurfaceKHR m_surface;       ///< surface
  bool m_usePresentation = false; ///< flag of presentation usage, true if surface is valid
  std::unique_ptr<vkb::Swapchain> m_swapchain; ///< swapchain
  std::vector<VkImage> m_swapchainImages;
  std::vector<VkImageView> m_swapchainImageViews;
  std::list<FrameData> m_frames; ///< framebuffers
  std::list<FrameData>::iterator m_currentFrame;

  std::unique_ptr<DefaultFramebuffer> m_defaultFramebuffer;

private:
  /// destroy and create new swapchain
  void InvalidateSwapchain();
  void InvalidateFramebuffer();
};

} // namespace RHI::vulkan
