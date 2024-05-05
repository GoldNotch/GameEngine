#pragma once
#include <Formatter.hpp>

#include "../VulkanContext.hpp"
#include "Pipeline.hpp"
#include "ShaderCompiler.hpp"

namespace vk::utils
{


struct PipelineBuilder final
{
  explicit PipelineBuilder(const VulkanContext & ctx) noexcept;

  template<typename VertexDataT>
  PipelineBuilder & SetShaderAPI()
  {
    bindings = ShaderAPIBuilder<VertexDataT>::BuildBindings();
    attributes = ShaderAPIBuilder<VertexDataT>::BuildAttributes();
    descriptors_layout = ShaderAPIBuilder<VertexDataT>::BuildDescriptorsLayout();
    return *this;
  }

  template<typename CharT, typename... Args>
  PipelineBuilder & AttachShader(vk::ShaderStageFlagBits stage, const CharT * path,
                                 Args &&... args);

  std::unique_ptr<Pipeline> Build(const IRenderer & renderer, const VkRenderPass & renderPass,
                                  uint32_t subpass_index) &;

private:
  const VulkanContext & context_owner; ///< context

  std::vector<VkDynamicState> dynamic_states;
  std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachments_info;
  std::vector<VkVertexInputBindingDescription> bindings;
  std::vector<VkVertexInputAttributeDescription> attributes;
  std::vector<VkDescriptorSetLayoutBinding> descriptors_layout;
  std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
  std::vector<vk::ShaderModule> shaders;


  VkPipelineDynamicStateCreateInfo dynamic_states_info{};
  VkPipelineVertexInputStateCreateInfo vertex_input_info{};
  VkPipelineInputAssemblyStateCreateInfo assembly_info{};
  VkPipelineViewportStateCreateInfo viewport_info{};
  VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
  VkPipelineRasterizationStateCreateInfo rasterization_info{};
  VkPipelineMultisampleStateCreateInfo multisampling_info{};
  VkPipelineColorBlendStateCreateInfo color_blend_info{};

private:
  PipelineBuilder(const PipelineBuilder &) = delete;
  PipelineBuilder & operator=(const PipelineBuilder &) = delete;
};


/// @brief constructor for Pipeline builder
PipelineBuilder::PipelineBuilder(const VulkanContext & ctx) noexcept
  : context_owner(ctx)
  , dynamic_states{VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR}
{
  // setup all default values to the structs
  dynamic_states_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

  vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  assembly_info.primitiveRestartEnable = VK_FALSE;

  viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewport_info.viewportCount = 1;
  viewport_info.scissorCount = 1;

  rasterization_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterization_info.depthClampEnable = VK_FALSE;
  rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
  rasterization_info.rasterizerDiscardEnable = VK_FALSE;
  rasterization_info.lineWidth = 1.0f;
  rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterization_info.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterization_info.depthBiasEnable = VK_FALSE;
  rasterization_info.depthBiasConstantFactor = 0.0f; // Optional
  rasterization_info.depthBiasClamp = 0.0f;          // Optional
  rasterization_info.depthBiasSlopeFactor = 0.0f;    // Optional

  multisampling_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling_info.sampleShadingEnable = VK_FALSE;
  multisampling_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampling_info.minSampleShading = 1.0f;          // Optional
  multisampling_info.pSampleMask = nullptr;            // Optional
  multisampling_info.alphaToCoverageEnable = VK_FALSE; // Optional
  multisampling_info.alphaToOneEnable = VK_FALSE;      // Optional

  auto && color_blend_attachment = color_blend_attachments_info.emplace_back();
  color_blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                          VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  color_blend_attachment.blendEnable = VK_FALSE;
  color_blend_attachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blend_attachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.colorBlendOp = VK_BLEND_OP_ADD;             // Optional
  color_blend_attachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;  // Optional
  color_blend_attachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
  color_blend_attachment.alphaBlendOp = VK_BLEND_OP_ADD;             // Optional


  color_blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  color_blend_info.logicOpEnable = VK_FALSE;
  color_blend_info.logicOp = VK_LOGIC_OP_COPY; // Optional
  color_blend_info.blendConstants[0] = 0.0f;   // Optional
  color_blend_info.blendConstants[1] = 0.0f;   // Optional
  color_blend_info.blendConstants[2] = 0.0f;   // Optional
  color_blend_info.blendConstants[3] = 0.0f;   // Optional
}


/// @brief build a vulkan pipeline and pipeline layout
std::unique_ptr<Pipeline> PipelineBuilder::Build(const IRenderer & renderer,
                                                 const VkRenderPass & renderPass,
                                                 uint32_t subpass_index) &
{
  // update all array data in structs
  dynamic_states_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
  dynamic_states_info.pDynamicStates = dynamic_states.data();

  vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
  vertex_input_info.pVertexBindingDescriptions = bindings.data(); // Optional
  vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
  vertex_input_info.pVertexAttributeDescriptions = attributes.data(); // Optional

  color_blend_info.attachmentCount = static_cast<uint32_t>(color_blend_attachments_info.size());
  color_blend_info.pAttachments = color_blend_attachments_info.data();

  // create descriptor set layout
  VkDescriptorSetLayoutCreateInfo dsetLayoutInfo{};
  dsetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  dsetLayoutInfo.bindingCount = static_cast<uint32_t>(descriptors_layout.size());
  dsetLayoutInfo.pBindings = descriptors_layout.data();

  VkDescriptorSetLayout descr_layout;
  if (auto res = vkCreateDescriptorSetLayout(context_owner.GetDevice(), &dsetLayoutInfo, nullptr,
                                             &descr_layout);
      res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create descriptor set layout - " << res);

  // create pipeline layout
  VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;            // Optional
  pipelineLayoutInfo.pSetLayouts = &descr_layout;   // Optional
  pipelineLayoutInfo.pushConstantRangeCount = 0;    // Optional
  pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

  VkPipelineLayout layout;
  if (auto res =
        vkCreatePipelineLayout(context_owner.GetDevice(), &pipelineLayoutInfo, nullptr, &layout);
      res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create pipeline layout - " << res);

  // create pipeline
  VkGraphicsPipelineCreateInfo pipeline_info{};
  pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipeline_info.layout = layout;
  pipeline_info.renderPass = renderPass;
  pipeline_info.subpass = subpass_index;
  //states
  pipeline_info.stageCount = static_cast<uint32_t>(shader_stages.size());
  pipeline_info.pStages = shader_stages.data();
  pipeline_info.pDynamicState = &dynamic_states_info;
  pipeline_info.pInputAssemblyState = &assembly_info;
  pipeline_info.pVertexInputState = &vertex_input_info;
  pipeline_info.pViewportState = &viewport_info;
  pipeline_info.pRasterizationState = &rasterization_info;
  pipeline_info.pMultisampleState = &multisampling_info;
  pipeline_info.pColorBlendState = &color_blend_info;
  pipeline_info.pDepthStencilState = nullptr;        // Optional
  pipeline_info.basePipelineHandle = VK_NULL_HANDLE; // Optional
  pipeline_info.basePipelineIndex = -1;              // Optional

  VkPipeline pipeline{};
  if (auto res = vkCreateGraphicsPipelines(context_owner.GetDevice(), VK_NULL_HANDLE, 1,
                                           &pipeline_info, nullptr, &pipeline);
      res != VK_SUCCESS)
    throw std::runtime_error(Formatter() << "Failed to create graphics pipeline - " << res);

  // cleanup shader modules
  for (auto && shader : shaders)
    vkDestroyShaderModule(context_owner.GetDevice(), shader, nullptr);

  auto result = std::unique_ptr<Pipeline>(
    new Pipeline(context_owner, renderer, pipeline, layout, descr_layout));

  // initialize object
  for (auto && layout_info : descriptors_layout)
    result->CreateUniformDescriptors(layout_info.descriptorType, layout_info.descriptorCount);
  return result;
}

/// @brief attaches shader to the pipeline
/// @param stage - id of stage
/// @param path to file where shader
/// @param ...args -compilation flags for shader
template<typename CharT, typename... Args>
inline PipelineBuilder & PipelineBuilder::AttachShader(vk::ShaderStageFlagBits stage,
                                                       const CharT * path, Args &&... args)
{
  // check that stage wasn't added before
  assert(std::find_if(shader_stages.begin(), shader_stages.end(),
                      [stage](const auto & info) {
                        return info.stage == static_cast<VkShaderStageFlagBits>(stage);
                      }) == shader_stages.end());

  auto && module = vk::shaders::BuildShaderModule(context_owner, vk::shaders::ReadSPIRV(path));
  shaders.push_back(module);
  VkPipelineShaderStageCreateInfo & info =
    shader_stages.emplace_back(VkPipelineShaderStageCreateInfo{});
  info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  info.stage = static_cast<VkShaderStageFlagBits>(stage);
  info.pName = "main";
  info.module = module;
  return *this;
}

} // namespace vk::utils
