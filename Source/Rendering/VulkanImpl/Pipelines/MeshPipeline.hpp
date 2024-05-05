#pragma once
#include <memory>

#include "../Types.hpp"


/// @brief Creates mesh pipeline
std::unique_ptr<IPipeline> CreateMeshPipeline(const VulkanContext & ctx, const IRenderer & renderer,
                                              const vk::RenderPass & renderPass,
                                              uint32_t subpass_index);
