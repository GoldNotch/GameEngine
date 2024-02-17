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
  Mesh::VertexData>::BuildBindings() noexcept
{
  std::vector<VkVertexInputBindingDescription> bindings;
  bindings.reserve(1);
  {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Mesh::VertexData);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(binding);
  }
  return bindings;
}

std::vector<VkVertexInputAttributeDescription> VertexStateDescriptionBuilder<
  Mesh::VertexData>::BuildAttributes() noexcept
{
  std::vector<VkVertexInputAttributeDescription> attributes;
  attributes.reserve(2);
  { // position attribute
    VkVertexInputAttributeDescription attribute{};
    attribute.binding = 0;
    attribute.location = 0;
    attribute.format = VK_FORMAT_R32G32_SFLOAT;
    attribute.offset = offsetof(Mesh::VertexData, pos);
    attributes.emplace_back(attribute);
  }
  { // color attribute
    VkVertexInputAttributeDescription attribute;
    attribute.binding = 0;
    attribute.location = 1;
    attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute.offset = offsetof(Mesh::VertexData, color);
    attributes.emplace_back(attribute);
  }
  return attributes;
}


struct MeshPipeline::Impl final
{
  Impl(const VulkanContext & ctx, const vk::RenderPass & renderPass, uint32_t subpass);
  ~Impl();

  void Process(const vk::CommandBuffer & buffer) const;

private:
  const VulkanContext & context;      ///< context-owner
  vk::Pipeline pipeline;              ///< pipeline handler
  vk::PipelineLayout pipeline_layout; ///< pipeline layout handler
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

void MeshPipeline::Process(const vk::CommandBuffer & buffer) const
{
  impl->Process(buffer);
}

/// @brief constructor, initialize all vulkan objects
/// @param ctx
MeshPipeline::Impl::Impl(const VulkanContext & ctx, const vk::RenderPass & renderPass,
                         uint32_t subpass_index)
  : context(ctx)
{
  vk::utils::PipelineBuilder builder{ctx};
  std::tie(pipeline, pipeline_layout) =
    builder.SetVertexData<Mesh::VertexData>()
      .AttachShader(vk::ShaderStageFlagBits::eVertex, shaders::vertex_shader.c_str())
      .AttachShader(vk::ShaderStageFlagBits::eFragment, shaders::fragment_shader.c_str())
      .Build(renderPass, subpass_index);
}

/// @brief destructor, destroys all vulkan objects
MeshPipeline::Impl::~Impl()
{
  context->destroyPipelineLayout(pipeline_layout, nullptr);
  context->destroyPipeline(pipeline, nullptr);
}

void MeshPipeline::Impl::Process(const vk::CommandBuffer & buffer) const
{
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
  VkViewport viewport{};
  viewport.x = 0.0f;
  viewport.y = 0.0f;
  viewport.width = static_cast<float>(800);
  viewport.height = static_cast<float>(600);
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(buffer, 0, 1, &viewport);

  VkRect2D scissor{};
  scissor.offset = {0, 0};
  scissor.extent = {800, 600};
  vkCmdSetScissor(buffer, 0, 1, &scissor);

  vkCmdDraw(buffer, 3, 1, 0, 0);
}
