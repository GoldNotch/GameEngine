#pragma once

#include "VulkanContext.hpp"

namespace RHI::vulkan
{
struct Framebuffer;
namespace details
{
struct PipelineBuilder;
struct PipelineLayoutBuilder;
struct DescriptorSetLayoutBuilder;
} // namespace details

struct Pipeline final : public IPipeline
{
  explicit Pipeline(const Context & ctx, const IFramebuffer & framebuffer, uint32_t subpassIndex);
  virtual ~Pipeline() override;

  virtual void AttachShader(ShaderType type, const wchar_t * path) override;
  virtual void AddInputBinding(uint32_t slot, uint32_t stride, InputBindingType type) override;
  virtual void AddInputAttribute(uint32_t binding, uint32_t location, uint32_t offset,
                                 uint32_t elemsCount, InputAttributeElementType elemsType) override;

  virtual void Invalidate() override;
  virtual uint32_t GetSubpass() const noexcept override { return m_subpassIndex; }

  vk::Pipeline GetPipelineHandle() const noexcept { return m_pipeline; }

private:
  const Context & m_owner;
  const IFramebuffer & m_framebuffer;
  uint32_t m_subpassIndex;
  vk::DescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
  vk::PipelineLayout m_layout = VK_NULL_HANDLE;
  vk::Pipeline m_pipeline = VK_NULL_HANDLE;

  std::unique_ptr<details::DescriptorSetLayoutBuilder> m_descriptorSetLayoutBuilder;
  std::unique_ptr<details::PipelineLayoutBuilder> m_layoutBuilder;
  std::unique_ptr<details::PipelineBuilder> m_pipelineBuilder;
  bool m_invalidPipeline : 1 = false;
  bool m_invalidPipelineLayout : 1 = false;
  bool m_invalidDescriptorSetLayout : 1 = false;
};

} // namespace RHI::vulkan
