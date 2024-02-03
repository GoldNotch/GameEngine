#include "MeshPipeline.hpp"

#include "ShadersCompiler.hpp"
#include "VulkanRendering.hpp"

namespace shaders
{
static constexpr std::string_view vertex_shader = "shaders/triangle_vert.spv";
static constexpr std::string_view fragment_shader = "shaders/triangle_frag.spv";
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


struct MeshPipeline::Impl final
{
  Impl(const VulkanContext & ctx, const vk::RenderPass & renderPass, uint32_t subpass);
  ~Impl();

  void Process(const vk::CommandBuffer & buffer) const;

private:
  const VulkanContext & context;      ///< context-owner
  vk::Pipeline pipeline;              ///< pipeline handler
  vk::PipelineLayout pipeline_layout; ///< pipeline layout handler

  ShaderProgram program{context};
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
  program.AttachShader(vk::ShaderStageFlagBits::eVertex, "Shaders/Vulkan/triangle_vert.spv")
    .AttachShader(vk::ShaderStageFlagBits::eFragment, "Shaders/Vulkan/triangle_frag.spv");

  VkGraphicsPipelineCreateInfo pipelineInfo{};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = program.GetStagesCount();
  pipelineInfo.pStages = program.GetStages();

  std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
  VkPipelineDynamicStateCreateInfo dynamicState{};
  dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
  dynamicState.pDynamicStates = dynamicStates.data();
  pipelineInfo.pDynamicState = &dynamicState;

  VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 0;
  vertexInputInfo.pVertexBindingDescriptions = nullptr; // Optional
  vertexInputInfo.vertexAttributeDescriptionCount = 0;
  vertexInputInfo.pVertexAttributeDescriptions = nullptr; // Optional
  pipelineInfo.pVertexInputState = &vertexInputInfo;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;
  pipelineInfo.pInputAssemblyState = &inputAssembly;

  VkPipelineViewportStateCreateInfo viewportState{};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.scissorCount = 1;
  pipelineInfo.pViewportState = &viewportState;

  VkPipelineRasterizationStateCreateInfo rasterizer{};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;
  rasterizer.depthBiasConstantFactor = 0.0f; // Optional
  rasterizer.depthBiasClamp = 0.0f;          // Optional
  rasterizer.depthBiasSlopeFactor = 0.0f;    // Optional
  pipelineInfo.pRasterizationState = &rasterizer;

  VkPipelineMultisampleStateCreateInfo multisampling{};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling.minSampleShading = 1.0f;          // Optional
  multisampling.pSampleMask = nullptr;            // Optional
  multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling.alphaToOneEnable = VK_FALSE;      // Optional
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = nullptr; // Optional

  VkPipelineColorBlendAttachmentState colorBlendAttachment{};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional

  VkPipelineColorBlendStateCreateInfo colorBlending{};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f; // Optional
  colorBlending.blendConstants[1] = 0.0f; // Optional
  colorBlending.blendConstants[2] = 0.0f; // Optional
  colorBlending.blendConstants[3] = 0.0f; // Optional
  pipelineInfo.pColorBlendState = &colorBlending;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 0;            // Optional
  pipelineLayoutInfo.pSetLayouts = nullptr;         // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
  VkPipelineLayout c_layout;
  if (vkCreatePipelineLayout(ctx.GetDevice(), &pipelineLayoutInfo, nullptr, &c_layout) !=
      VK_SUCCESS)
    throw std::runtime_error("failed to create pipeline layout!");
  pipeline_layout = vk::PipelineLayout(c_layout);
  pipelineInfo.layout = pipeline_layout;

  pipelineInfo.renderPass = renderPass;
  pipelineInfo.subpass = subpass_index;

  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipelineInfo.basePipelineIndex = -1;              // Optional


  VkPipeline c_pipe{};
  if (auto res = vkCreateGraphicsPipelines(ctx.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo,
                                           nullptr, &c_pipe);
      res != VK_SUCCESS)
  {
    throw std::runtime_error("failed to create graphics pipeline!");
  }
  pipeline = vk::Pipeline(c_pipe);
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
