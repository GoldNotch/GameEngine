#include <list>

#include "../RenderingSystem.h"

//#define LOGGING_IMPLEMENTATION
#include <Logging.hpp>
//IMPLEMENT_LOGGING_API(Rendering, RENDERING_API)

#include "VulkanContext.hpp"




// ------------------------ API implementation ----------------------------

int RENDERING_API InitRenderingSystem(usRenderingOptions opts)
{
  std::list<VulkanContext> st_contexts; ///< each context for each GPU
  //io::Log(LogStatus::US_LOG_INFO, 0, "Initialize Vulkan");
  try
  {
    auto && ctx = st_contexts.emplace_back(opts);
  }
  catch (...)
  {
    TerminateRenderingSystem();
    return -1;
  }
  //io::Log(LogStatus::US_LOG_INFO, 0, "Vulkan is initialized successfully");
  return 0;
}

/// @brief clear all resources for rendering system
void RENDERING_API TerminateRenderingSystem()
{
  //st_contexts.clear();
}

void RENDERING_API RenderFrame(const void * const scene)
{
  /*for (auto && ctx : st_contexts)
    ctx.GetRenderer()->Render();*/
}
