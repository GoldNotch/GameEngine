#pragma once
#include <memory>

#include "Types.hpp"
struct VulkanContext;
namespace vk
{
class RenderPass;
}

struct Mesh
{
};

/// @brief graphics pipeline
struct MeshPipeline final : public IPipeline
{
  MeshPipeline(const VulkanContext & ctx, const vk::RenderPass & renderPass,
               uint32_t subpass_index);
  virtual ~MeshPipeline() override;

  virtual void Process(const vk::CommandBuffer & buffer) const override;

private:
  /// @brief implementation incapsulates vulkan calls
  struct Impl;
  std::unique_ptr<Impl> impl;

private:
  MeshPipeline(const MeshPipeline &) = delete;
  MeshPipeline & operator=(const MeshPipeline &) = delete;
};

template<>
struct SubpassDescriptionBuilder<MeshPipeline> final
{
  static vk::SubpassDescription Get() noexcept;
};
