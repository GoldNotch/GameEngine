#ifndef RENDERING_H
#define RENDERING_H

#ifdef _WIN32
#if defined(RENDERING_STATIC_BUILD)
#define RENDERING_API
#elif defined(RENDERING_BUILD)
#define RENDERING_API __declspec(dllexport)
#else
#define RENDERING_API __declspec(dllimport)
#endif

#else
#define RENDERING_API
#endif

#include <LoggingAPI.h>
DECLARE_LOGGING_API(Rendering, RENDERING_API)

#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
#include <Windows.h> // for HWND, HINSTANCE
#else

#endif
#endif // USE_WINDOW_OUTPUT

#include "Scene.h"


#ifdef __cplusplus
extern "C"
{
#endif

  /// @brief max count of used gpus in app
  const size_t MaxRequiredGPUs = 1;

  /// @brief options for rendering system initializing
  typedef struct
  {
    bool gpu_autodetect;
    size_t required_gpus;
    const char * gpu_name[MaxRequiredGPUs];
#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
    HWND hWindow;
    HINSTANCE hInstance;
#else

#endif
#endif // USE_WINDOW_OUTPUT

  } RenderingSystemConfig;

  /// @brief rendering system handler
  typedef void * RenderingSystem;

  /// @brief create and init rendering system
  /// @param config
  RENDERING_API RenderingSystem CreateRenderingSystem(RenderingSystemConfig config);

  /// @brief destroy all objects and clear all resources in rendering system
  RENDERING_API void DestroyRenderingSystem(RenderingSystem system);

  /// @brief Invalidates render resources like framebuffers (call it when change resolution)
  RENDERING_API void Invalidate(RenderingSystem system);

  /// @brief render scene
  /// @param scene
  RENDERING_API void RenderFrame(RenderingSystem system);

    /// @brief gets render scene container
  RENDERING_API RenderSceneHandler AcquireRenderScene(RenderingSystem system);


#ifdef __cplusplus
}
#endif

#endif // RENDERING_H
