#pragma once

namespace vk
{
class Pipeline;
class PipelineLayout;
} // namespace vk

struct PipelineBuilder final
{
  std::pair<vk::Pipeline, vk::PipelineLayout> build();
};
