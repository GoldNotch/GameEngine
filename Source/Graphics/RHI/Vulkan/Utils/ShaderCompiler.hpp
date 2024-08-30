#pragma once
#include <codecvt>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include <vulkan/vulkan.hpp>

namespace RHI::vulkan::details
{

/// @brief reads shader SPIR-V file as binary
/// @param filename - path to file
/// @return vector of bytes
std::string ReadSPIRV(const std::filesystem::path & path)
{
  thread_local std::locale loc("en_US.UTF-8");

  std::wifstream file(path.c_str(), std::ios::in | std::ios::binary);
  file.imbue(loc);
  if (!file.is_open() && file.good())
    throw std::runtime_error("failed to open file!");

  std::stringstream ss;
  ss << file.rdbuf();

  file.close();
  return ss.str();
}


/// @brief creates shader module in context by filename
/// @param ctx - vulkan context
/// @param filename - path to file
/// @return compiled shader module
vk::ShaderModule BuildShaderModule(const vk::Device & device, const std::filesystem::path & path)
{
  auto code = ReadSPIRV(path);
  VkShaderModuleCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  createInfo.codeSize = code.size();
  createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

  VkShaderModule module;
  if (VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &module);
      result != VK_SUCCESS)
    throw std::runtime_error("failed to create shader module!");
  return vk::ShaderModule(module);
}

} // namespace RHI::vulkan::details
