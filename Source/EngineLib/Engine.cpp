#include "Engine.h"

#include <cassert>
#include <ctime>
#include <memory>
#include <queue>
#include <vector>

#include "Rendering/RenderingSystem.hpp"
#include "Rendering/Swapchain.hpp"
#define LOGGING_IMPLEMENTATION
#include "Core/Logging.hpp"
#include "Core/Storage.hpp"


/// @brief packs 4 separate bytes into int
/// @param r - red channel
/// @param g - green channel
/// @param b - blue channel
/// @param a - alpha channel
/// @return
inline color_t RGB2UINT(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  return (a << 24u) + (b << 16u) + (g << 8u) + r;
}

/// @brief engine global data
struct USEngine final
{
  std::vector<color_t> result_image;
  usVideoOptions current_opts = {0, 0};
  USEngine() = default;
  ~USEngine() = default;
  USEngine(USEngine &&) = default;
  USEngine & operator=(USEngine &&) = default;

  RenderableScene & GetScene() { return scene; }
  const RenderableScene & GetScene() const { return scene; }

private:
  RenderableScene scene;
  //render::Swapchain swapchain;  ///< queue for ready frames
  // swap chain for frames
  // queue for app-thread communication

private:
  USEngine(const USEngine &) = delete;
  USEngine & operator=(const USEngine &) = delete;
};
static std::unique_ptr<USEngine> st_engine = nullptr;

// ---------------------- API implementation -------------------------------

/// @brief create and initialize Engine instance
/// @param options
/// @return true if initialization is succeed
USENGINE_API bool usEngineInit(usConstructOptions options)
{
  srand(time(NULL));
  try
  {
    st_engine = std::make_unique<USEngine>();
    io::Log(US_LOG_INFO, 0, "Initialize rendering system");
    InitRenderingSystem(options.rendering_options);
  }
  catch (std::exception e)
  {
    io::Log(US_LOG_INFO, 0, "Error occured in engine initialization");
    return false;
  }
  return true;
}

/// @brief terminate engine and clear all resources
USENGINE_API void usEngineTerminate()
{
  TerminateRenderingSystem();
  st_engine.reset();
}


/// @brief command to start frame processing
/// @param opts - frame options
/// @return
USENGINE_API usRenderableSceneHandler usBeginFrame(usVideoOptions opts)
{
  st_engine->result_image.reserve(opts.width * opts.height);
  st_engine->current_opts = opts;
  // maybe it should block app-thread to process scene and collect draw data
  return &st_engine->GetScene();
}

/// @brief block thread-caller untill frame is ready to return
/// @return frame data
USENGINE_API usFrame usRenderFrame()
{
  Render2D(RenderableScene{});
  return usFrame{};
}

/// @brief clear all frame resources
USENGINE_API void usEndFrame()
{
  st_engine->result_image.clear();
  st_engine->GetScene().clear();
}

USENGINE_API void usRenderableScene_ConstructMeshObject(usRenderableSceneHandler handler,
                                                        usMeshObjectConstructorArgs args)
{
  RenderableScene & scene = *reinterpret_cast<RenderableScene *>(handler);
  scene.emplace(args);
}
