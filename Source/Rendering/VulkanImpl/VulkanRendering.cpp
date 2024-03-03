#include <list>
#include <queue>

#include "../RenderingSystem.h"

#define LOGGING_IMPLEMENTATION
#include <Logging.hpp>
IMPLEMENT_LOGGING_API(Rendering, RENDERING_API)


#include "VulkanContext.hpp"


static std::list<VulkanContext> st_contexts; ///< each context for each GPU
static std::queue<RenderScene> st_scenes;
// ------------------------ API implementation ----------------------------

RENDERING_API int InitRenderingSystem(usRenderingOptions opts)
{
  io::Log(LogStatus::US_LOG_INFO, 0, "Initialize Vulkan");
  try
  {
    auto && ctx = st_contexts.emplace_back(opts);
  }
  catch (std::exception e)
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
  while (!st_scenes.empty())
    st_scenes.pop();
  st_contexts.clear();
}

RENDERING_API void Invalidate()
{
  for (auto && ctx : st_contexts)
    ctx.GetRenderer()->Invalidate();
}

RENDERING_API void RenderFrame()
{
  for (auto && ctx : st_contexts)
    ctx.GetRenderer()->Render(st_scenes.front());
  st_scenes.pop();
}


RENDERING_API RenderSceneHandler AcquireRenderScene()
{
  auto && new_scene = st_scenes.emplace();
  return &new_scene;
}

RENDERING_API void RenderScene_PushStaticMesh(RenderSceneHandler scene, StaticMesh mesh)
{
  auto && last_scene = st_scenes.back();
  last_scene.objects.emplace<StaticMesh>(mesh);
}
