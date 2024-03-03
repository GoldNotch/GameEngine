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
