#include "Pipeline.hpp"

#include "Framebuffer.hpp"
#include "Utils/Builders.hpp"
namespace RHI::vulkan
{

Pipeline::Pipeline(const Context & ctx, const IFramebuffer & framebuffer, uint32_t subpassIndex)
  : m_owner(ctx)
  , m_framebuffer(framebuffer)
  , m_subpassIndex(subpassIndex)
{
  m_descriptorSetLayoutBuilder = std::make_unique<details::DescriptorSetLayoutBuilder>();
  m_layoutBuilder = std::make_unique<details::PipelineLayoutBuilder>();
  m_pipelineBuilder = std::make_unique<details::PipelineBuilder>();
}

Pipeline::~Pipeline()
{
  if (!!m_pipeline)
    vkDestroyPipeline(m_owner.GetDevice(), m_pipeline, nullptr);
  if (!!m_layout)
    vkDestroyPipelineLayout(m_owner.GetDevice(), m_layout, nullptr);
  if (!!m_descriptorSetLayout)
    vkDestroyDescriptorSetLayout(m_owner.GetDevice(), m_descriptorSetLayout, nullptr);
}

void Pipeline::AttachShader(ShaderType type, const wchar_t * path)
{
  m_pipelineBuilder->AttachShader(type, path);
  m_invalidPipeline = true;
}

void Pipeline::Invalidate()
{
  if (m_invalidDescriptorSetLayout || !m_descriptorSetLayout)
  {
    auto new_layout = m_descriptorSetLayoutBuilder->Make(m_owner.GetDevice());
    if (!!m_descriptorSetLayout)
      vkDestroyDescriptorSetLayout(m_owner.GetDevice(), m_descriptorSetLayout, nullptr);
    m_descriptorSetLayout = new_layout;
  }

  if (m_invalidDescriptorSetLayout || m_invalidPipelineLayout || !m_layout)
  {
    auto new_layout = m_layoutBuilder->Make(m_owner.GetDevice(), m_descriptorSetLayout);
    if (!!m_layout)
      vkDestroyPipelineLayout(m_owner.GetDevice(), m_layout, nullptr);
    m_layout = new_layout;
  }

  if (m_invalidPipeline || m_invalidPipelineLayout || m_invalidDescriptorSetLayout || !m_pipeline)
  {
    auto new_pipeline =
      m_pipelineBuilder->Make(m_owner.GetDevice(),
                              reinterpret_cast<VkRenderPass>(m_framebuffer.GetRenderPass()),
                              m_subpassIndex, m_layout);
    if (!!m_pipeline)
      vkDestroyPipeline(m_owner.GetDevice(), m_pipeline, nullptr);
    m_pipeline = new_pipeline;
  }

  m_invalidPipeline = false;
  m_invalidPipelineLayout = false;
  m_invalidDescriptorSetLayout = false;
}

} // namespace RHI::vulkan
