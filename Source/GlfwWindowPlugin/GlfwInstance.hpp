#pragma once

namespace GlfwWindowsPlugin
{
struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
  double GetTimestamp() const;
};

} // namespace Utils
