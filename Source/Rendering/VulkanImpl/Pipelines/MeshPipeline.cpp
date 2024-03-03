#include "MeshPipeline.hpp"

#include <unordered_map>

#include "../Utils/PipelineBuilder.hpp"
#include "../VulkanContext.hpp"

namespace shaders
{
static constexpr auto vertex_shader = vk::utils::ResolveShaderPath("triangle_vert.spv");
static constexpr auto fragment_shader = vk::utils::ResolveShaderPath("triangle_frag.spv");
} // namespace shaders


/// @brief hash function for StaticMeshData
template<>
struct std::hash<StaticMesh>
{
  size_t operator()(StaticMesh const & mesh) const noexcept
  {
    size_t hash = 0;
    Core::utils::hash_combine(hash, mesh.vertices_count);
    Core::utils::hash_combine(hash, mesh.vertices);
    return hash;
  }
};

/// @brief equality operator for StaticMeshData
bool operator==(const StaticMesh & m1, const StaticMesh & m2)
{
  return std::memcmp(&m1, &m2, sizeof(StaticMesh)) == 0;
}


/// @brief pipeline for StaticMeshData
struct MeshPipeline final : public IPipeline
{
  /// @brief constructor
  /// @param ctx - vulkan context
  /// @param renderPass - vk::RenderPass
  /// @param subpass_index - index of subpass in render pass
  MeshPipeline(const VulkanContext & ctx, const vk::RenderPass & renderPass,
               uint32_t subpass_index);

  /// @brief destructor
  virtual ~MeshPipeline() noexcept override;

  /// @brief some logic in the beginning of processing (f.e. some preparations to rendering)
  /// @param buffer - command buffer
  /// @param viewport - settings for pipeline
  virtual void BeginProcessing(const vk::CommandBuffer & buffer,
                               const vk::Rect2D & viewport) const override;

  /// @brief Some logic in the end of processing (f.e. flush all commands in buffer)
  /// @param buffer - command buffer
  virtual void EndProcessing(const vk::CommandBuffer & buffer) const override;

  /// @brief pipeline-specific function to process one drawable object
  /// @param buffer - command buffer
  /// @param mesh - drawable data
  void ProcessObject(const vk::CommandBuffer & buffer, const StaticMesh & mesh) const;

private:
  const VulkanContext & context;                           ///< vulkan context
  VulkanHandler pipeline;                                  ///< vulkan pipeline
  VulkanHandler layout;                                    ///< vulkan pipeline layout
  mutable std::unordered_map<StaticMesh, BufferGPU> cache; ///< cache of bufferized geometry

private:
  MeshPipeline(const MeshPipeline &) = delete;
  MeshPipeline & operator=(const MeshPipeline &) = delete;
};


// ------------------------ Implementation -----------------------

/// @brief constructor, initialize all vulkan objects
/// @param ctx
MeshPipeline::MeshPipeline(const VulkanContext & ctx, const vk::RenderPass & renderPass,
                           uint32_t subpass_index)
  : context(ctx)
{
  vk::utils::PipelineBuilder builder{ctx};
  std::tie(pipeline, layout) =
    builder.SetVertexData<StaticMesh>()
      .AttachShader(vk::ShaderStageFlagBits::eVertex, shaders::vertex_shader.c_str())
      .AttachShader(vk::ShaderStageFlagBits::eFragment, shaders::fragment_shader.c_str())
      .Build(renderPass, subpass_index);
}

/// @brief destructor, destroys all vulkan objects
MeshPipeline::~MeshPipeline() noexcept
{
  context->destroyPipelineLayout(reinterpret_cast<VkPipelineLayout>(layout), nullptr);
  context->destroyPipeline(reinterpret_cast<VkPipeline>(pipeline), nullptr);
}

void MeshPipeline::BeginProcessing(const vk::CommandBuffer & buffer, const vk::Rect2D & vp) const
{
  vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    reinterpret_cast<VkPipeline>(pipeline));
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
}

void MeshPipeline::ProcessObject(const vk::CommandBuffer & buffer, const StaticMesh & mesh) const
{
  auto cache_it = cache.find(mesh);
  if (cache_it == cache.end())
  {
    size_t size = mesh.vertices_count * (sizeof(glVec2) + sizeof(glVec3));
    auto new_buffer =
      context.GetMemoryManager().AllocBuffer(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    {
      auto ptr = new_buffer.Map();
      size_t offset = 0;
      std::memcpy(reinterpret_cast<char *>(ptr.get()) + offset, mesh.vertices,
                  mesh.vertices_count * sizeof(glVec2));
      offset += mesh.vertices_count * sizeof(glVec2);
      std::memcpy(reinterpret_cast<char *>(ptr.get()) + offset, mesh.colors,
                  mesh.vertices_count * sizeof(glVec3));
    }
    new_buffer.Flush();
    cache_it = cache.insert(cache.end(), std::make_pair(mesh, std::move(new_buffer)));
  }
  VkBuffer vertexBuffers[] = {static_cast<vk::Buffer>(cache_it->second),
                              static_cast<vk::Buffer>(cache_it->second)};
  VkDeviceSize offsets[] = {0, mesh.vertices_count * sizeof(glVec2)};
  vkCmdBindVertexBuffers(buffer, 0, 2, vertexBuffers, offsets);

  vkCmdDraw(buffer, static_cast<uint32_t>(mesh.vertices_count), 1, 0, 0);
}

void MeshPipeline::EndProcessing(const vk::CommandBuffer & buffer) const
{
}

// ----------------- Pipeline template specializations -------------------------

template<>
void ProcessWithPipeline<StaticMesh>(const IPipeline & pipeline, const vk::CommandBuffer & buffer,
                                     const StaticMesh & obj)
{
  assert(typeid(pipeline) == typeid(MeshPipeline));
  static_cast<const MeshPipeline &>(pipeline).ProcessObject(buffer, obj);
}

vk::SubpassDescription SubpassDescriptionBuilder<StaticMesh>::Get() noexcept
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
  StaticMesh>::BuildBindings() noexcept
{
  std::vector<VkVertexInputBindingDescription> bindings;
  bindings.reserve(1);
  {
    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(glVec2);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(binding);
  }
  {
    VkVertexInputBindingDescription binding{};
    binding.binding = 1;
    binding.stride = sizeof(glVec3);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    bindings.emplace_back(binding);
  }
  return bindings;
}

std::vector<VkVertexInputAttributeDescription> VertexStateDescriptionBuilder<
  StaticMesh>::BuildAttributes() noexcept
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
    attribute.binding = 1;
    attribute.location = 1;
    attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute.offset = 0;
    attributes.emplace_back(attribute);
  }
  return attributes;
}

// -----------------------------------------------------------------------

std::unique_ptr<IPipeline> CreateMeshPipeline(const VulkanContext & ctx,
                                              const vk::RenderPass & renderPass,
                                              uint32_t subpass_index)
{
  return std::make_unique<MeshPipeline>(ctx, renderPass, subpass_index);
}
