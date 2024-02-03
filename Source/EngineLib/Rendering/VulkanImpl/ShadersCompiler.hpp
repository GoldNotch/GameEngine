#pragma once
#include <fstream>
#include <string>
#include <vector>

#include "VulkanRendering.hpp"


struct ShaderProgram
{
  ShaderProgram(const VulkanContext & ctx)
    : context_owner(ctx)
  {
  }
  ~ShaderProgram()
  {
    for (auto && shader : shaders)
      context_owner->destroyShaderModule(shader, nullptr);
  }

  template<typename CharT, typename... Args>
  ShaderProgram & AttachShader(vk::ShaderStageFlagBits stage, const CharT * path, Args &&... args)
  {
    // check that stage wasn't added before
    assert(std::find_if(stages.begin(), stages.end(),
                        [stage](const auto & info) {
                          return info.stage == static_cast<VkShaderStageFlagBits>(stage);
                        }) == stages.end());
    auto && module = buildShaderModule(context_owner, readFile(path));
    shaders.push_back(module);
    VkPipelineShaderStageCreateInfo & info = stages.emplace_back(VkPipelineShaderStageCreateInfo{});
    info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    info.stage = static_cast<VkShaderStageFlagBits>(stage);
    info.pName = "main";
    info.module = module;
    return *this;
  }

  uint32_t GetStagesCount() const { return static_cast<uint32_t>(stages.size()); }
  const VkPipelineShaderStageCreateInfo * GetStages() const { return stages.data(); }

private:
  const VulkanContext & context_owner;
  std::vector<VkPipelineShaderStageCreateInfo> stages;
  std::vector<vk::ShaderModule> shaders;

private:
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram & operator=(const ShaderProgram &) = delete;

private:
  template<typename CharT>
  static std::vector<char> readFile(const CharT * path);
  static vk::ShaderModule buildShaderModule(const VulkanContext & ctx,
                                            const std::vector<char> & code);
};


/// @brief reads shader SPIR-V file as binary
/// @param filename - path to file
/// @return vector of bytes
template<typename CharT>
std::vector<char> ShaderProgram::readFile(const CharT * path)
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
vk::ShaderModule ShaderProgram::buildShaderModule(const VulkanContext & ctx,
                                                  const std::vector<char> & code)
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
