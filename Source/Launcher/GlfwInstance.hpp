#pragma once

namespace Utils
{
struct GlfwInstance final
{
  GlfwInstance();
  ~GlfwInstance();
  void PollEvents();
  double GetTimestamp() const;
};

} // namespace Utils
