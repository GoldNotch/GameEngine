#include "Swapchain.hpp"

#include <Formatter.hpp>
#include <VkBootstrap.h>

namespace RHI::vulkan
{

FrameData::FrameData(const Context & ctx, VkFormat swapchainFormat)
  : m_owner(ctx)
{
  std::tie(m_queueIndex, m_queue) = ctx.GetQueue(QueueType::Graphics);
  image_available_semaphore = utils::CreateVkSemaphore(ctx.GetDevice());
  render_finished_semaphore = utils::CreateVkSemaphore(ctx.GetDevice());
  is_rendering = utils::CreateFence(ctx.GetDevice(), true /*locked*/);
}

FrameData::~FrameData()
{
  vkDestroySemaphore(m_owner.GetDevice(), image_available_semaphore, nullptr);
  vkDestroySemaphore(m_owner.GetDevice(), render_finished_semaphore, nullptr);
  vkDestroyFence(m_owner.GetDevice(), is_rendering, nullptr);
}

void FrameData::WaitForRenderingComplete() const
{
  vkWaitForFences(m_owner.GetDevice(), 1, &is_rendering, VK_TRUE, UINT64_MAX);
}

void FrameData::Submit(std::vector<CommandBufferHandle> && buffers) const
{
  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &image_available_semaphore;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = static_cast<uint32_t>(buffers.size());
  submitInfo.pCommandBuffers = reinterpret_cast<VkCommandBuffer *>(buffers.data());

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &render_finished_semaphore;

  if (auto res = vkQueueSubmit(m_queue, 1, &submitInfo, is_rendering); res != VK_SUCCESS)
    throw std::runtime_error("failed to submit draw command buffer!");
}

// -------------------------- Renderer ---------------------------

Swapchain::Swapchain(const Context & ctx, const vk::SurfaceKHR surface)
  : m_owner(ctx)
  , m_surface(surface)
  , m_swapchain(std::make_unique<vkb::Swapchain>())
{
  m_usePresentation = !!surface;

  InvalidateSwapchain();
  m_defaultFramebuffer = std::make_unique<DefaultFramebuffer>(ctx, m_swapchain->image_count,
                                                              m_swapchain->image_format,
                                                              VK_SAMPLE_COUNT_1_BIT);
  InvalidateFramebuffer();

  // create frames
  for (uint32_t i = 0; i < m_swapchain->image_count; ++i)
    auto && frame = m_frames.emplace_back(m_owner, m_swapchain->image_format);

  m_currentFrame = m_frames.begin();
}

Swapchain::~Swapchain()
{
  //vkDestroyRenderPass(context_owner.GetDevice(), render_pass, nullptr);
  m_swapchain->destroy_image_views(m_swapchainImageViews);
  vkb::destroy_swapchain(*m_swapchain);
  vkb::destroy_surface(m_owner.GetInstance(), m_surface);
}

void Swapchain::Invalidate()
{
  if (m_usePresentation)
  {
    vkDeviceWaitIdle(m_owner.GetDevice());
    InvalidateSwapchain();
    InvalidateFramebuffer();

    //auto it = m_frames.begin();
    //for (uint32_t i = 0; i < m_swapchain->image_count; ++i)
    //{
    //  it->Invalidate(m_swapchainImageViews[i], m_swapchain->extent);
    //  it = std::next(it);
    //}
    m_currentFrame = m_frames.begin();
  }
}

void Swapchain::InvalidateSwapchain()
{
  if (m_usePresentation)
  {
    std::tie(m_presentFamilyIndex, m_presentQueue) = m_owner.GetQueue(QueueType::Present);
    vkb::SwapchainBuilder swapchain_builder(m_owner.GetGPU(), m_owner.GetDevice(), m_surface,
                                            m_renderFamilyIndex, m_presentFamilyIndex);
    auto swap_ret = swapchain_builder.set_old_swapchain(*m_swapchain).build();
    if (!swap_ret)
    {
      throw std::runtime_error(Formatter() << "Failed to create Vulkan swapchain - "
                                           << swap_ret.error().message());
    }

    if (!m_swapchainImageViews.empty())
      m_swapchain->destroy_image_views(m_swapchainImageViews);

    vkb::destroy_swapchain(*m_swapchain);

    *m_swapchain = swap_ret.value();
    m_swapchainImages = m_swapchain->get_images().value();
    m_swapchainImageViews = m_swapchain->get_image_views().value();
  }
  //else offscreen rendering
}

void Swapchain::InvalidateFramebuffer()
{
  auto && extent = m_swapchain->extent;
  m_defaultFramebuffer->SetExtent(extent.width, extent.height);
  m_defaultFramebuffer->SetSwapchainImages(m_swapchainImageViews);
  m_defaultFramebuffer->Invalidate();
}

void Swapchain::SwapBuffers()
{
  m_currentFrame = std::next(m_currentFrame);
  if (m_currentFrame == m_frames.end())
    m_currentFrame = m_frames.begin();
  m_defaultFramebuffer->OnSwapBuffers();
}

const IFramebuffer & Swapchain::GetDefaultFramebuffer() const & noexcept
{
  return *m_defaultFramebuffer;
}
uint32_t Swapchain::GetBuffersCount() const
{
  return m_swapchain->image_count;
}
} // namespace RHI::vulkan
