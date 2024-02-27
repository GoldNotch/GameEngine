#include "MeshPipeline.hpp"

#include "Utils/PipelineBuilder.hpp"
#include "VulkanContext.hpp"

namespace shaders
{
static constexpr auto vertex_shader = vk::utils::ResolveShaderPath("triangle_vert.spv");
static constexpr auto fragment_shader = vk::utils::ResolveShaderPath("triangle_frag.spv");
} // namespace shaders


vk::SubpassDescription SubpassDescriptionBuilder<MeshPipeline>::Get() noexcept
{
  VkSubpassDescription result{};
  static std::vector<VkAttachmentReference> attachmentsRef;
  if (attachmentsRef.empty())
  {
    auto && colorAttachment = attachmentsRef.emplace_back(VkAttachmentReference{});
    colorAttachment.attachment = 0;
    colorAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  }

  result.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  result.colorAttachmentCount = static_cast<uint32_t>(attachmentsRef.size());
  result.pColorAttachments = attachmentsRef.data();
  return result;
}

std::vector<VkVertexInputBindingDescription> VertexStateDescriptionBuilder<
  Mesh>::BuildBindings() noexcept
{
  std::vector<VkVertexInputBindingDescription> bindings;
  bindings.reserve(1);
  {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(glVec3) + sizeof(glVec2);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(binding);
  }
  return bindings;
}

std::vector<VkVertexInputAttributeDescription> VertexStateDescriptionBuilder<
  Mesh>::BuildAttributes() noexcept
{
  std::vector<VkVertexInputAttributeDescription> attributes;
  attributes.reserve(2);
  { // position attribute
    VkVertexInputAttributeDescription attribute{};
    attribute.binding = 0;
    attribute.location = 0;
    attribute.format = VK_FORMAT_R32G32_SFLOAT;
    attribute.offset = 0;
    attributes.emplace_back(attribute);
  }
  { // color attribute
    VkVertexInputAttributeDescription attribute;
    attribute.binding = 0;
    attribute.location = 1;
    attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute.offset = sizeof(glVec2); //offsetof(Mesh::VertexData, color);
    attributes.emplace_back(attribute);
  }
  return attributes;
}


struct MeshPipeline::Impl final
{
  Impl(const VulkanContext & ctx, const vk::RenderPass & renderPass, uint32_t subpass);
  ~Impl();

  void Process(const vk::Rect2D & viewport, const vk::CommandBuffer & buffer) const;

private:
  const VulkanContext & context;      ///< context-owner
  vk::Pipeline pipeline;              ///< pipeline handler
  vk::PipelineLayout pipeline_layout; ///< pipeline layout handler

  MemoryManager::BufferGPU buffer{};
};


/// @brief constructor for pipeline
MeshPipeline::MeshPipeline(const VulkanContext & ctx, const vk::RenderPass & renderPass,
                           uint32_t subpass_index)
  : impl(new Impl(ctx, renderPass, subpass_index))
{
}

/// @brief pipeline destructor
MeshPipeline::~MeshPipeline()
{
  impl.reset();
}


void MeshPipeline::Process(const vk::Rect2D & viewport, const vk::CommandBuffer & buffer) const
{
  impl->Process(viewport, buffer);
}


// ------------------------ Implementation -----------------------

/// @brief constructor, initialize all vulkan objects
/// @param ctx
MeshPipeline::Impl::Impl(const VulkanContext & ctx, const vk::RenderPass & renderPass,
                         uint32_t subpass_index)
  : context(ctx)
{
  vk::utils::PipelineBuilder builder{ctx};
  std::tie(pipeline, pipeline_layout) =
    builder.SetVertexData<Mesh>()
      .AttachShader(vk::ShaderStageFlagBits::eVertex, shaders::vertex_shader.c_str())
      .AttachShader(vk::ShaderStageFlagBits::eFragment, shaders::fragment_shader.c_str())
      .Build(renderPass, subpass_index);

  static const std::vector<float> vertices = {0.0f, -0.5f, 1.0,   0.0,  0.0,  0.5f, 0.5f, 0.0f,
                                              1.0f, 0.0f,  -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};

  auto new_buffer = ctx.GetMemoryManager().AllocBuffer(vertices.size() * sizeof(float),
                                              VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
  {
    auto ptr = new_buffer.Map();
    std::memcpy(ptr.get(), vertices.data(), vertices.size() * sizeof(float));
  }
  new_buffer.Flush();
  buffer = std::move(new_buffer);
}

/// @brief destructor, destroys all vulkan objects
MeshPipeline::Impl::~Impl()
{
  context->destroyPipelineLayout(pipeline_layout, nullptr);
  context->destroyPipeline(pipeline, nullptr);
}

void MeshPipeline::Impl::Process(const vk::Rect2D & vp,
                                 const vk::CommandBuffer & buffer) const
{
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  VkViewport viewport{};
  viewport.x = static_cast<float>(vp.offset.x);
  viewport.y = static_cast<float>(vp.offset.y);
  viewport.width = static_cast<float>(vp.extent.width);
  viewport.height = static_cast<float>(vp.extent.height);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(buffer, 0, 1, &viewport);

  VkRect2D scissor = vp;
  vkCmdSetScissor(buffer, 0, 1, &scissor);

  VkBuffer vertexBuffers[] = {static_cast<vk::Buffer>(this->buffer)};
  VkDeviceSize offsets[] = {0};
  vkCmdBindVertexBuffers(buffer, 0, 1, vertexBuffers, offsets);

  vkCmdDraw(buffer, 3, 1, 0, 0);
}
