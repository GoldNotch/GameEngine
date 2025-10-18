#pragma once

namespace Utils
{
struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
};

} // namespace Utils
