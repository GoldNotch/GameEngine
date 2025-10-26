#pragma once

namespace GlfwPlugin
{
struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
  double GetTimestamp() const;
};

} // namespace Utils
