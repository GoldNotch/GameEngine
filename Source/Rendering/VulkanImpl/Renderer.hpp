#pragma once
#include <memory>

#include "Types.hpp"

namespace vk
{
class CommandBuffer;
class SurfaceKHR;
} // namespace vk
struct VulkanContext;


std::unique_ptr<IRenderer> CreateRenderer(const VulkanContext & ctx,
                                          const vk::SurfaceKHR & surface);

//------------------- Pipelines ---------------------

/// @brief Creates mesh pipeline
std::unique_ptr<IPipeline> CreateMeshPipeline(const VulkanContext & ctx, const IRenderer & renderer,
                                              const vk::RenderPass & renderPass,
                                              uint32_t subpass_index);
