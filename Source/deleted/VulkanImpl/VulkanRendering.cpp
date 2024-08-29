#include <list>
#include <queue>

#include "../RenderingSystem.h"

#define LOGGING_IMPLEMENTATION
#include <Logging.hpp>
IMPLEMENT_LOGGING_API(Rendering, RENDERING_API)


#include "VulkanContext.hpp"


struct VulkanRenderingSystem final
{
  explicit VulkanRenderingSystem(const RenderingSystemConfig & config)
    : ctx(config)
  {
  }

  void Invalidate() { ctx.GetRenderer()->Invalidate(); }
  void RenderFrame()
  {
    if (scenes.empty())
      return;
    ctx.GetRenderer()->Render(scenes.front());
    scenes.pop();
  }

  RenderScene & AcquireRenderScene() & { return scenes.emplace(); }

private:
  VulkanContext ctx;
  std::queue<RenderScene> scenes;
};


// ------------------------ API implementation ----------------------------

RENDERING_API RenderingSystem CreateRenderingSystem(RenderingSystemConfig config)
{
  io::Log(LogStatus::US_LOG_INFO, 0, "Initialize Vulkan rendering system");
  RenderingSystem system = std::make_unique<VulkanRenderingSystem>(config).release();
  io::Log(LogStatus::US_LOG_INFO, 0, "Vulkan rendering system is initialized successfully");
  return system;
}

/// @brief clear all resources for rendering system
RENDERING_API void DestroyRenderingSystem(RenderingSystem system)
{
  delete reinterpret_cast<VulkanRenderingSystem *>(system);
}

RENDERING_API void Invalidate(RenderingSystem system)
{
  reinterpret_cast<VulkanRenderingSystem *>(system)->Invalidate();
}

RENDERING_API void RenderFrame(RenderingSystem system)
{
  reinterpret_cast<VulkanRenderingSystem *>(system)->RenderFrame();
}

RENDERING_API RenderSceneHandler AcquireRenderScene(RenderingSystem system)
{
  return &reinterpret_cast<VulkanRenderingSystem *>(system)->AcquireRenderScene();
}

RENDERING_API void RenderScene_PushStaticMesh(RenderSceneHandler scene, StaticMesh mesh)
{
  auto * last_scene = reinterpret_cast<RenderScene *>(scene);
  last_scene->objects.emplace<StaticMesh>(mesh);
}
