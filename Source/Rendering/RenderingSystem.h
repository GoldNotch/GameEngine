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

#ifdef __cplusplus
extern "C"
{
#endif

  typedef struct
  {
    float x, y;
  } glVec2;
  typedef struct
  {
    float x, y, z;
  } glVec3;
  typedef struct
  {
    float x, y, z, w;
  } glVec4;

  typedef struct MeshData
  {
    size_t vertices_count;
    const glVec3 * vertices;
    const glVec4 * colors;

    size_t indices_count;
    const size_t * indices;
  } Mesh;


  typedef unsigned int color_t;
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

  } usRenderingOptions;


  /// @brief create and init rendering system
  /// @param opts
  RENDERING_API int InitRenderingSystem(usRenderingOptions opts);

  /// @brief destroy all objects and clear all resources in rendering system
  RENDERING_API void TerminateRenderingSystem();

  /// @brief render scene
  /// @param scene
  RENDERING_API void RenderFrame(const void * const scene);

  RENDERING_API void Invalidate();

#ifdef __cplusplus
}
#endif

#endif // RENDERING_H
