#pragma once
#include <filesystem>
#include <vector>

#include <Context.hpp>

namespace vk
{
class RenderPass;
class Framebuffer;
class Device;
class ImageView;
struct Extent;
} // namespace vk

struct VkAttachmentDescription;
struct VkAttachmentReference;
struct VkSubpassDescription;
struct VkSubpassDependency;

namespace RHI::vulkan::details
{

struct RenderPassBuilder final
{
  using SubpassSlots = std::vector<std::pair<ShaderImageSlot, uint32_t>>;
  void AddAttachment(const VkAttachmentDescription & description);
  void AddSubpass(SubpassSlots slotsLayout);

  vk::RenderPass Make(const vk::Device & device) const;
  void Reset();

private:
  std::vector<SubpassSlots> m_slots;
  std::vector<VkAttachmentDescription> m_attachments;
};

} // namespace RHI::vulkan::details


namespace RHI::vulkan::details
{
struct FramebufferBuilder final
{
  void SetRenderPass(const vk::RenderPass & renderPass);
  void SetExtent(uint32_t width, uint32_t height);
  void AddAttachment(const vk::ImageView & image);
  vk::ImageView & SetAttachment(size_t idx) & noexcept;
  vk::Framebuffer Make(const vk::Device & device) const;
  void Reset();

private:
  std::vector<vk::ImageView> m_images;
  vk::Extent2D m_extent{0, 0};
  vk::RenderPass m_renderPass;
};

} // namespace RHI::vulkan::details


namespace RHI::vulkan::details
{
struct DescriptorSetLayoutBuilder final
{
  vk::DescriptorSetLayout Make(const vk::Device & device) const;
  void Reset();
  //void AddBinding();

private:
  std::vector<VkDescriptorSetLayoutBinding> m_descriptorsLayout;
};


struct PipelineLayoutBuilder final
{
  vk::PipelineLayout Make(const vk::Device & device, const vk::DescriptorSetLayout & layout) const;
  void Reset() { m_layouts.clear(); }

private:
  std::vector<VkDescriptorSetLayout> m_layouts;
};


/// @brief Utility-class to automatize pipeline building. It provides default values to many settings nad methods to configure your pipeline
struct PipelineBuilder final
{
  PipelineBuilder();
  vk::Pipeline Make(const vk::Device & device, const VkRenderPass & renderPass,
                    uint32_t subpass_index, const VkPipelineLayout & layout);
  void Reset();

  void AttachShader(RHI::ShaderType type, const std::filesystem::path & path);
  void SetMeshTopology(MeshTopology topology) { m_topology = topology; }
  void SetLineWidth(float width) { m_lineWidth = width; }
  void SetPolygonMode(PolygonMode mode) { m_polygonMode = mode; }
  void SetCullingMode(CullingMode mode, FrontFace front)
  {
    m_cullingMode = mode;
    m_frontFace = front;
  }

  void SetBlendEnabled(bool value) { m_blendEnabled = value; }


private:
  MeshTopology m_topology = MeshTopology::Triangle;

  float m_lineWidth = 1.0f;
  PolygonMode m_polygonMode;

  CullingMode m_cullingMode;
  FrontFace m_frontFace;

  bool m_blendEnabled;
  BlendOperation m_blendColorOp;
  BlendOperation m_blendAlphaOp;
  BlendFactor m_blendSrcColorFactor;
  BlendFactor m_blendDstColorFactor;
  BlendFactor m_blendSrcAlphaFactor;
  BlendFactor m_blendDstAlphaFactor;

  std::vector<std::pair<ShaderType, std::filesystem::path>> m_attachedShaders;

  std::vector<VkDynamicState> m_dynamicStates;
  std::vector<VkPipelineColorBlendAttachmentState> m_colorBlendAttachments;
  std::vector<VkVertexInputBindingDescription> m_bindings;
  std::vector<VkVertexInputAttributeDescription> m_attributes;

private:
  PipelineBuilder(const PipelineBuilder &) = delete;
  PipelineBuilder & operator=(const PipelineBuilder &) = delete;
};
} // namespace RHI::vulkan::details
