#include "Renderer.hpp"

#include <iterator>
#include <list>

#include <Formatter.hpp>
#include <Logging.hpp>

#include "Pipelines/MeshPipeline.hpp"
#include "VulkanContext.hpp"

namespace
{
/// @brief represents data for each frame in swapchain
struct FrameData final
{
  FrameData(const VulkanContext & ctx, VkRenderPass render_pass, VkImageView view,
            VkExtent2D img_extent)
    : context_owner(ctx)
    , render_pass(render_pass)
  {
    std::tie(render_queue_index, render_queue) = ctx.GetQueue(vkb::QueueType::graphics);
    image_available_semaphore = ctx.CreateVkSemaphore();
    render_finished_semaphore = ctx.CreateVkSemaphore();
    is_rendering = ctx.CreateFence(true /*locked*/);
    Invalidate(view, img_extent);
  }

  ~FrameData()
  {
    vkDestroyCommandPool(context_owner.GetDevice(), pool, nullptr);
    vkDestroySemaphore(context_owner.GetDevice(), image_available_semaphore, nullptr);
    vkDestroySemaphore(context_owner.GetDevice(), render_finished_semaphore, nullptr);
    vkDestroyFence(context_owner.GetDevice(), is_rendering, nullptr);
    vkDestroyFramebuffer(context_owner.GetDevice(), framebuffer, nullptr);
  }

  const VkFramebuffer & GetFramebuffer() const & { return framebuffer; }
  const VkSemaphore & GetImageAvailableSemaphore() const & { return image_available_semaphore; }
  const VkSemaphore & GetRenderingFinishedSemaphore() const & { return render_finished_semaphore; }
  const VkFence & GetRenderingFence() const & { return is_rendering; }
  const VkCommandBuffer & GetCommandBuffer() const & { return buffer; }

  void WaitOldRenderingComplete() const
  {
    vkWaitForFences(context_owner.GetDevice(), 1, &is_rendering, VK_TRUE, UINT64_MAX);
  }
  void BeginRender() const;
  void EndRender() const;
  void Invalidate(VkImageView view, VkExtent2D new_img_extent);

private:
  const VulkanContext & context_owner; ///< context, doesn't own
  VkRenderPass render_pass;            ///< render_pass - owner for framebuffer
  VkQueue render_queue;
  uint32_t render_queue_index;

  // owned data
  VkFramebuffer framebuffer = VK_NULL_HANDLE;
  VkSemaphore image_available_semaphore;
  VkSemaphore render_finished_semaphore;
  VkFence is_rendering;
  VkCommandPool pool = VK_NULL_HANDLE;
  VkCommandBuffer buffer = VK_NULL_HANDLE;

private:
  FrameData(const FrameData &) = delete;
  FrameData & operator=(const FrameData &) = delete;
};

void FrameData::BeginRender() const
{
  // reset fence
  vkResetFences(context_owner.GetDevice(), 1, &is_rendering);
  vkResetCommandBuffer(buffer, 0);

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0;                  // Optional
  beginInfo.pInheritanceInfo = nullptr; // Optional

  if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("failed to begin recording command buffer!");
}


void FrameData::EndRender() const
{
  if (vkEndCommandBuffer(buffer) != VK_SUCCESS)
    throw std::runtime_error("failed to record command buffer!");

  VkSubmitInfo submitInfo{};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

  VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
  submitInfo.waitSemaphoreCount = 1;
  submitInfo.pWaitSemaphores = &image_available_semaphore;
  submitInfo.pWaitDstStageMask = waitStages;
  submitInfo.commandBufferCount = 1;
  VkCommandBuffer buffers[] = {buffer};
  submitInfo.pCommandBuffers = buffers;

  submitInfo.signalSemaphoreCount = 1;
  submitInfo.pSignalSemaphores = &render_finished_semaphore;

  if (auto res = vkQueueSubmit(render_queue, 1, &submitInfo, is_rendering); res != VK_SUCCESS)
    throw std::runtime_error("failed to submit draw command buffer!");
}


void FrameData::Invalidate(VkImageView view, VkExtent2D new_img_extent)
{
  VkImageView attachments[] = {view};
  VkFramebufferCreateInfo framebufferInfo{};
  framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  framebufferInfo.renderPass = render_pass;
  framebufferInfo.attachmentCount = 1;
  framebufferInfo.pAttachments = attachments;
  framebufferInfo.width = new_img_extent.width;
  framebufferInfo.height = new_img_extent.height;
  framebufferInfo.layers = 1;
  VkFramebuffer new_framebuffer;
  if (vkCreateFramebuffer(context_owner.GetDevice(), &framebufferInfo, nullptr, &new_framebuffer) !=
      VK_SUCCESS)
  {
    throw std::runtime_error("failed to create framebuffer!");
  }

  if (framebuffer != VK_NULL_HANDLE)
    vkDestroyFramebuffer(context_owner.GetDevice(), framebuffer, nullptr);
  framebuffer = new_framebuffer;

  if (pool != VK_NULL_HANDLE)
    vkDestroyCommandPool(context_owner.GetDevice(), pool, nullptr);

  auto new_pool = context_owner.CreateCommandPool(render_queue_index);
  auto new_buffer = context_owner.CreateCommandBuffer(new_pool);
  pool = new_pool;
  buffer = new_buffer;
}

/// @brief creates render pass
template<typename... Args>
vk::RenderPass CreateRenderPass(const VulkanContext & ctx, VkFormat image_format)
{
  VkRenderPass renderPass{};
  VkAttachmentDescription colorAttachment{};
  colorAttachment.format = image_format;
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
    throw std::runtime_error(Formatter() << "Failed to create render pass - " << res);

  return vk::RenderPass(renderPass);
}

} // namespace


/// @brief vulkan implementation for renderer
struct Renderer final : public IRenderer
{
  explicit Renderer(const VulkanContext & ctx, const vk::SurfaceKHR surface);
  virtual ~Renderer() override;

  virtual void Render(const RenderScene & scene) override;

  /// @brief destroys old surface data like framebuffers, images, images_views, ets and creates new
  virtual void Invalidate() override;

private:
  const VulkanContext & context_owner;
  vk::RenderPass render_pass; ///< render pass

  vk::Queue render_queue = VK_NULL_HANDLE;
  uint32_t render_family_index;
  vk::Queue present_queue = VK_NULL_HANDLE;
  uint32_t present_family_index;

  /// presentaqtion data
  vk::SurfaceKHR surface;                ///< surface
  bool use_presentation = false;         ///< flag of presentation usage, true if surface is valid
  vkb::Swapchain swapchain;              ///< swapchain
  std::vector<VkImage> swapchain_images; ///< images for swapchain
  //TODO: make FrameData own imageview
  std::vector<VkImageView> swapchain_imageviews; ///< image views for swapchain
  std::list<FrameData> frames;                   ///< framebuffers
  std::list<FrameData>::iterator current_frame;

  // pipelines
  //TODO: make it with interface
  std::unique_ptr<IPipeline> pipeline = nullptr;

private:
  /// destroy and create new swapchain
  void InvalidateSwapchain();
};

// ------------------------------- Implementation ------------------------

Renderer::Renderer(const VulkanContext & ctx, const vk::SurfaceKHR surface)
  : context_owner(ctx)
  , surface(surface)
{
  use_presentation = surface != VK_NULL_HANDLE;

  InvalidateSwapchain();

  render_pass = CreateRenderPass<StaticMesh>(ctx, swapchain.image_format);

  // create pipelines
  pipeline = CreateMeshPipeline(ctx, render_pass, 0);

  // create frames
  for (auto && view : swapchain_imageviews)
    frames.emplace_back(context_owner, render_pass, view, swapchain.extent);
  current_frame = frames.begin();
}

Renderer::~Renderer()
{
  pipeline.reset();
  vkDestroyRenderPass(context_owner.GetDevice(), render_pass, nullptr);
  swapchain.destroy_image_views(swapchain_imageviews);
  vkb::destroy_swapchain(swapchain);
  vkb::destroy_surface(context_owner.GetInstance(), surface);
}

/// @brief renders one frame
void Renderer::Render(const RenderScene & scene)
{
  current_frame->WaitOldRenderingComplete();
  uint32_t image_index = 0;

  if (use_presentation)
  {
    VkResult res = vkAcquireNextImageKHR(context_owner.GetDevice(), swapchain, UINT64_MAX,
                                         current_frame->GetImageAvailableSemaphore(),
                                         VK_NULL_HANDLE, &image_index);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    {
      Invalidate();
      return;
    }
    else if (res != VK_SUCCESS)
    {
      io::Log(US_LOG_ERROR, res, "Failed to acquire swap chain image");
      return;
    }
  }
  //else offscreen rendering

  assert(image_index == std::distance(frames.begin(), current_frame));

  current_frame->BeginRender();

  VkRenderPassBeginInfo renderPassInfo{};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = render_pass;
  renderPassInfo.framebuffer = current_frame->GetFramebuffer();
  renderPassInfo.renderArea.offset = {0, 0};
  renderPassInfo.renderArea.extent = swapchain.extent;

  VkClearValue clearColor = {{{0.0f, 200.5f, 0.5f, 1.0f}}};
  renderPassInfo.clearValueCount = 1;
  renderPassInfo.pClearValues = &clearColor;

  vkCmdBeginRenderPass(current_frame->GetCommandBuffer(), &renderPassInfo,
                       VK_SUBPASS_CONTENTS_INLINE);
  pipeline->BeginProcessing(current_frame->GetCommandBuffer(),
                            vk::Rect2D({0, 0}, swapchain.extent));

  for (auto it = scene.objects.cbegin<StaticMesh>(); it != scene.objects.cend<StaticMesh>(); ++it)
    ProcessWithPipeline(*pipeline, current_frame->GetCommandBuffer(), *it);

  pipeline->EndProcessing(current_frame->GetCommandBuffer());
  vkCmdEndRenderPass(current_frame->GetCommandBuffer());

  current_frame->EndRender();

  if (use_presentation)
  {
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &current_frame->GetRenderingFinishedSemaphore();

    VkSwapchainKHR swapChains[] = {swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &image_index;
    presentInfo.pResults = nullptr; // Optional
    VkResult res = vkQueuePresentKHR(present_queue, &presentInfo);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
    {
      Invalidate();
      return;
    }
    else if (res != VK_SUCCESS)
    {
      io::Log(US_LOG_ERROR, res, "Failed to queue image in present");
    }
  }

  // requires current_frame was mutable
  current_frame = std::next(current_frame);
  if (current_frame == frames.end())
    current_frame = frames.begin();
}


/// @brief destroys all surface resources: swapchain, framebuffers, etc and creates new
void Renderer::Invalidate()
{
  if (use_presentation)
  {
    vkDeviceWaitIdle(context_owner.GetDevice());
    InvalidateSwapchain();
    size_t i = 0;
    for (auto && frame : frames)
      frame.Invalidate(swapchain_imageviews[i++], swapchain.extent);
    current_frame = frames.begin();
  }
}

/// @brief destroys old swapchain, images, image_views and creates new
void Renderer::InvalidateSwapchain()
{
  if (use_presentation)
  {
    std::tie(present_family_index, present_queue) = context_owner.GetQueue(vkb::QueueType::present);
    vkb::SwapchainBuilder swapchain_builder(context_owner.GetGPU(), context_owner.GetDevice(),
                                            surface, render_family_index, present_family_index);
    auto swap_ret = swapchain_builder.set_old_swapchain(swapchain).build();
    if (!swap_ret)
    {
      throw std::runtime_error(Formatter() << "Failed to create Vulkan swapchain - "
                                           << swap_ret.error().message());
    }

    if (!swapchain_imageviews.empty())
      swapchain.destroy_image_views(swapchain_imageviews);

    vkb::destroy_swapchain(swapchain);
    swapchain = swap_ret.value();

    swapchain_images = swapchain.get_images().value();
    swapchain_imageviews = swapchain.get_image_views().value();
  }
  //else offscreen rendering
}

std::unique_ptr<IRenderer> CreateRenderer(const VulkanContext & ctx, const vk::SurfaceKHR & surface)
{
  return std::make_unique<Renderer>(ctx, surface);
}
