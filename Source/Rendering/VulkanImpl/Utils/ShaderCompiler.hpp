#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "../VulkanContext.hpp"

namespace vk::shaders
{


/// @brief reads shader SPIR-V file as binary
/// @param filename - path to file
/// @return vector of bytes
template<typename CharT>
static std::vector<char> ReadSPIRV(const CharT * path)
{
  std::ifstream file(path, std::ios::in | std::ios::binary);

  if (!file.is_open() && file.good())
    throw std::runtime_error("failed to open file!");


  std::vector<char> result((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
  file.close();
  return result;
}


/// @brief creates shader module in context by filename
/// @param ctx - vulkan context
/// @param filename - path to file
/// @return compiled shader module
static vk::ShaderModule BuildShaderModule(const VulkanContext & ctx, const std::vector<char> & code)
{
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule module;
  if (VkResult result = vkCreateShaderModule(ctx.GetDevice(), &createInfo, nullptr, &module);
      result != VK_SUCCESS)
    throw std::runtime_error("failed to create shader module!");
  return vk::ShaderModule(module);
}

} // namespace vk::shaders
