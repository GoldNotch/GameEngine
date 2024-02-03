#include "Renderer.hpp"

#include "../../Core/Logging.hpp"
#include "MeshPipeline.hpp"
#include "VulkanRendering.hpp"

namespace
{

template<typename... Args>
vk::RenderPass CreateRenderPass(const VulkanContext & ctx, const vkb::Swapchain & swapchain)
{
  VkRenderPass renderPass{};
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = swapchain.image_format;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // MSAA
  colorAttachment.loadOp =
    VK_ATTACHMENT_LOAD_OP_CLEAR; // action for color/depth buffers in pass begins
  colorAttachment.storeOp =
    VK_ATTACHMENT_STORE_OP_STORE; // action for color/depth buffers in pass ends
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // action
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


  std::vector<VkSubpassDescription> subpasses = {SubpassDescriptionBuilder<Args>::Get()...};

  VkSubpassDependency dependency{};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  ;

  VkRenderPassCreateInfo renderPassCreateInfo{};
  renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassCreateInfo.attachmentCount = 1;
  renderPassCreateInfo.pAttachments = &colorAttachment;
  renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
  renderPassCreateInfo.pSubpasses = subpasses.data();
  renderPassCreateInfo.dependencyCount = 1;
  renderPassCreateInfo.pDependencies = &dependency;


  if (auto res = vkCreateRenderPass(ctx.GetDevice(), &renderPassCreateInfo, nullptr, &renderPass);
      res != VK_SUCCESS)
  {
    io::Log(US_LOG_ERROR, 0, "Failed to create render pass - %i", res);
  }

  return vk::RenderPass(renderPass);
}

} // namespace

struct Renderer2D::Impl final
{
  explicit Impl(const VulkanContext & ctx, const vk::SurfaceKHR surface);
  ~Impl();

  void Render() const;

private:
  //TODO: move vulkan data into Impl object in .cpp file
  const VulkanContext & context_owner;
  vk::RenderPass render_pass; ///< render pass

  vk::Queue render_queue = VK_NULL_HANDLE;
  uint32_t render_family_index;
  vk::Queue present_queue = VK_NULL_HANDLE;
  uint32_t present_family_index;

  vk::CommandPool pool;
  vk::CommandBuffer buffer;

  /// presentaqtion data
  vk::SurfaceKHR surface;        ///< surface
  bool use_presentation = false; ///< flag of presentation usage, true if surface is valid
  vkb::Swapchain swapchain;      ///< swapchain
  //TODO: swapchain images wrap with class
  std::vector<VkImage> swapchain_images;             ///< images for swapchain
  std::vector<VkImageView> swapchain_imageviews;     ///< image views for swapchain
  std::vector<VkFramebuffer> swapchain_framebuffers; ///< main framebuffer

  vk::Semaphore image_available_semaphore;
  vk::Semaphore render_finished_semaphore;
  vk::Fence in_flight_fence;

  // pipelines
  //TODO: make it with interface
  std::unique_ptr<MeshPipeline> pipeline = nullptr;

private:
  /// @brief initialize framebuffer with created render_pass
  /// @param render_pass
  void InitFramebuffers();
};

Renderer2D::Renderer2D(const VulkanContext & ctx, const vk::SurfaceKHR & surface)
  : impl(new Impl(ctx, surface))
{
}

Renderer2D::~Renderer2D()
{
  impl.reset();
}

void Renderer2D::Render() const
{
  impl->Render();
}

// ------------------------------- Implementation ------------------------

Renderer2D::Impl::Impl(const VulkanContext & ctx, const vk::SurfaceKHR surface)
  : context_owner(ctx)
  , surface(surface)
{
  use_presentation = surface != VK_NULL_HANDLE;
  std::tie(render_family_index, render_queue) = ctx.GetQueue(vkb::QueueType::graphics);

  if (use_presentation)
  {
    std::tie(present_family_index, present_queue) = ctx.GetQueue(vkb::QueueType::present);
    vkb::SwapchainBuilder swapchain_builder(ctx.GetGPU(), ctx.GetDevice(), surface,
                                            render_family_index, present_family_index);
    auto swap_ret = swapchain_builder.set_old_swapchain(swapchain).build();
    if (!swap_ret)
    {
      io::Log(US_LOG_ERROR, 0, "Failed to create Vulkan swapchain - %s",
              swap_ret.error().message());
    }
    vkb::destroy_swapchain(swapchain);
    swapchain = swap_ret.value();

    swapchain_images = swapchain.get_images().value();
    swapchain_imageviews = swapchain.get_image_views().value();
  }
  //else offscreen rendering
  pool = ctx.CreateCommandPool(render_family_index);
  buffer = context_owner.CreateCommandBuffer(pool);
  render_pass = CreateRenderPass<MeshPipeline>(ctx, swapchain);
  pipeline = std::make_unique<MeshPipeline>(ctx, render_pass, 0);
  InitFramebuffers();
  image_available_semaphore = ctx.CreateVkSemaphore();
  render_finished_semaphore = ctx.CreateVkSemaphore();
  in_flight_fence = ctx.CreateFence(true /*locked*/);
}

Renderer2D::Impl::~Impl()
{
  context_owner->destroySemaphore(image_available_semaphore, nullptr);
  context_owner->destroySemaphore(render_finished_semaphore, nullptr);
  context_owner->destroyFence(in_flight_fence, nullptr);
  context_owner->destroyRenderPass(render_pass, nullptr);
  swapchain.destroy_image_views(swapchain_imageviews);
  for (auto && fbo : swapchain_framebuffers)
    context_owner->destroyFramebuffer(fbo, nullptr);
  context_owner->destroyCommandPool(pool, nullptr);
  vkb::destroy_swapchain(swapchain);
  vkb::destroy_surface(context_owner.GetInstance(), surface);
}

/// @brief renders one frame
void Renderer2D::Impl::Render() const
{
  VkFence fence = in_flight_fence;
  vkWaitForFences(context_owner.GetDevice(), 1, &fence, VK_TRUE, UINT64_MAX);
  vkResetFences(context_owner.GetDevice(), 1, &fence);
  vkResetCommandBuffer(buffer, 0);

  uint32_t image_index = 0;
  if (use_presentation)
  {
    VkResult res = vkAcquireNextImageKHR(context_owner.GetDevice(), swapchain, UINT64_MAX,
                                                      image_available_semaphore, VK_NULL_HANDLE,
                                                      &image_index);
    if (res != VK_SUCCESS)
      throw std::runtime_error("failed to acquire image");
  }
  //else offscreen rendering

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;                  // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("failed to begin recording command buffer!");

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass;
  renderPassInfo.framebuffer = swapchain_framebuffers[image_index];
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchain.extent;

  VkClearValue clearColor = {{{0.0f, 200.5f, 0.5f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
  pipeline->Process(buffer);
  vkCmdEndRenderPass(buffer);

  if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
    throw std::runtime_error("failed to record command buffer!");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkSemaphore waitSemaphores[] = {image_available_semaphore};
  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = waitSemaphores;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer buffers[] = {buffer};
  submitInfo.pCommandBuffers = buffers;

  VkSemaphore signalSemaphores[] = {render_finished_semaphore};
  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = signalSemaphores;

  if (vkQueueSubmit(render_queue, 1, &submitInfo, in_flight_fence) != VK_SUCCESS)
  {
    throw std::runtime_error("failed to submit draw command buffer!");
  }

  if (use_presentation)
  {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = nullptr; // Optional
    if (VkResult res = vkQueuePresentKHR(present_queue, &presentInfo); res != VK_SUCCESS)
      throw std::runtime_error("failed to query present");
  }
}


void Renderer2D::Impl::InitFramebuffers()
{
  swapchain_framebuffers.resize(swapchain_imageviews.size());
  for (size_t i = 0, c = swapchain_imageviews.size(); i < c; ++i)
  {
    VkImageView attachments[] = {swapchain_imageviews[i]};
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = render_pass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = swapchain.extent.width;
    framebufferInfo.height = swapchain.extent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(context_owner.GetDevice(), &framebufferInfo, nullptr,
                            &swapchain_framebuffers[i]) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create framebuffer!");
    }
  }
}
