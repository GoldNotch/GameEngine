#include "GlfwInstance.hpp"

#include <cstdio>

#include <GLFW/glfw3.h>

namespace
{

void OnGlfwError(int code, const char * description)
{
  std::printf("Glfw Error %i - %s", code, description);
}

} // namespace

namespace Utils
{

GlfwInstance::GlfwInstance()
{
  glfwInit();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwSetErrorCallback(::OnGlfwError);
}

GlfwInstance::~GlfwInstance()
{
  glfwTerminate();
}

void GlfwInstance::PollEvents()
{
  glfwPollEvents();
}


} // namespace Utils
