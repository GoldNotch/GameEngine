#pragma once
#include <memory>

#include "Types.hpp"

namespace vk
{
class CommandBuffer;
class SurfaceKHR;
} // namespace vk
struct VulkanContext;


std::unique_ptr<ISwapchain> CreateRenderer(const VulkanContext & ctx,
                                          const vk::SurfaceKHR & surface);

//------------------- Pipelines ---------------------

/// @brief Creates mesh pipeline
std::unique_ptr<IPipeline> CreateMeshPipeline(const VulkanContext & ctx, const ISwapchain & renderer,
                                              const vk::RenderPass & renderPass,
                                              uint32_t subpass_index);
