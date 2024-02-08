#include <list>

#include "../RenderingSystem.h"

#define LOGGING_IMPLEMENTATION
#include <Logging.hpp>
IMPLEMENT_LOGGING_API(Rendering, RENDERING_API)

#include "VulkanContext.hpp"


static std::list<VulkanContext> st_contexts; ///< each context for each GPU
// ------------------------ API implementation ----------------------------

RENDERING_API int InitRenderingSystem(usRenderingOptions opts)
{
  io::Log(LogStatus::US_LOG_INFO, 0, "Initialize Vulkan");
  try
  {
    auto && ctx = st_contexts.emplace_back(opts);
  }
  catch (...)
  {
    TerminateRenderingSystem();
    return -1;
  }
  io::Log(LogStatus::US_LOG_INFO, 0, "Vulkan is initialized successfully");
  return 0;
}

/// @brief clear all resources for rendering system
RENDERING_API void TerminateRenderingSystem()
{
  st_contexts.clear();
}

RENDERING_API void RenderFrame(const void * const scene)
{
  for (auto && ctx : st_contexts)
    ctx.GetRenderer()->Render();
}
