#include "CommandBuffer.hpp"

#include "Pipeline.hpp"

namespace
{
vk::CommandBuffer CreateCommandBuffer(vk::Device device, vk::CommandPool pool,
                                      RHI::CommandBufferType type)
{
  VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
  VkCommandBufferAllocateInfo allocInfo{};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = pool;
  allocInfo.level = type == RHI::CommandBufferType::Executable ? VK_COMMAND_BUFFER_LEVEL_PRIMARY
                                                               : VK_COMMAND_BUFFER_LEVEL_SECONDARY;
  allocInfo.commandBufferCount = 1;

  if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
    throw std::runtime_error("failed to allocate command buffers!");

  return vk::CommandBuffer{commandBuffer};
}
} // namespace

namespace RHI::vulkan
{

CommandBuffer::CommandBuffer(vk::Device device, vk::CommandPool pool, CommandBufferType type)
  : m_buffer(CreateCommandBuffer(device, pool, type))
  , m_type(type)
{
}


void CommandBuffer::BeginWritingInSwapchain()
{
  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = 0; // Optional
  beginInfo.pInheritanceInfo = nullptr;
  if (vkBeginCommandBuffer(m_buffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("failed to begin recording command buffer!");
}

void CommandBuffer::BeginWriting(const IFramebuffer & framebuffer, const IPipeline & pipeline)
{
  VkCommandBufferInheritanceInfo inheritanceInfo{};
  if (m_type == CommandBufferType::ThreadLocal)
  {
    inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
    inheritanceInfo.renderPass = reinterpret_cast<VkRenderPass>(framebuffer.GetRenderPass());
    //inheritanceInfo.framebuffer = reinterpret_cast<VkFramebuffer>(framebuffer.GetHandle());
    inheritanceInfo.subpass = pipeline.GetSubpass();
  }

  VkCommandBufferBeginInfo beginInfo{};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = m_type == CommandBufferType::ThreadLocal
                    ? VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT |
                        VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT
                    : 0; // Optional
  beginInfo.pInheritanceInfo = m_type == CommandBufferType::ThreadLocal ? &inheritanceInfo
                                                                        : nullptr;
  if (vkBeginCommandBuffer(m_buffer, &beginInfo) != VK_SUCCESS)
    throw std::runtime_error("failed to begin recording command buffer!");

  auto && vkPipeline = static_cast<const Pipeline &>(pipeline);
  vkCmdBindPipeline(m_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline.GetPipelineHandle());
}

void CommandBuffer::EndWriting()
{
  if (vkEndCommandBuffer(m_buffer) != VK_SUCCESS)
    throw std::runtime_error("failed to record command buffer!");
}

void CommandBuffer::DrawVertices(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex,
                                 uint32_t firstInstance) const
{
  vkCmdDraw(m_buffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void CommandBuffer::SetViewport(float width, float height)
{
  VkViewport viewport = {};
  viewport.height = height;
  viewport.width = width;
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  vkCmdSetViewport(m_buffer, 0, 1, &viewport);
}

void CommandBuffer::SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height)
{
  VkRect2D scissor = {};
  scissor.extent.width = width;
  scissor.extent.height = height;
  scissor.offset.x = x;
  scissor.offset.y = y;
  vkCmdSetScissor(m_buffer, 0, 1, &scissor);
}

void CommandBuffer::Reset() const
{
  vkResetCommandBuffer(m_buffer, 0);
}

void CommandBuffer::AddCommands(const ICommandBuffer & buffer) const
{
  assert(buffer.GetType() == CommandBufferType::ThreadLocal);
  VkCommandBuffer buf = static_cast<const CommandBuffer &>(buffer).m_buffer;
  vkCmdExecuteCommands(m_buffer, 1, &buf);
}

} // namespace RHI::vulkan
