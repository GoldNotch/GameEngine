
#ifndef ENGINE_H
#define ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#if defined(ENGINE_STATIC_BUILD)
#define USENGINE_API
#elif defined(ENGINE_BUILD)
#define USENGINE_API __declspec(dllexport)
#else
#define USENGINE_API __declspec(dllimport)
#endif
#elif
#define USENGINE_API
#endif

#ifdef USE_WINDOW_OUTPUT
#ifdef _WIN32
#include <Windows.h> // for HWND, HINSTANCE
#else

#endif
#endif

  // ------------------------- Engine API --------------------------

  typedef unsigned int color_t;
  /// @brief max count of used gpus in app
  const size_t MaxRequiredGPUs = 1;

  /// @brief Options for frame's picture (width, height resolution, maybe channels, etc)
  typedef struct
  {
    int width, height;
  } usVideoOptions;

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
#endif

  } usRenderingOptions;

  /// @brief options for engine constructing
  typedef struct
  {
    usRenderingOptions rendering_options;
  } usConstructOptions;

  /// @brief result of one frame returned from engine
  typedef struct
  {
    const color_t * video_frame;
    usVideoOptions opts;
  } usFrame;

  /*
Engine is object creates frames.
One frame is a picture and sound sample
*/

  /// @brief initialize engine module
  /// @param options for creation
  /// @return true if success
  USENGINE_API bool usEngineInit(usConstructOptions options);
  /// @brief terminates engine module and clear all resources
  USENGINE_API void usEngineTerminate();

  /// @brief Types of log messages
  typedef enum
  {
    US_LOG_INFO,       ///< use for information, debugging
    US_LOG_WARNING,    ///< use for strange situations which are should be kept in eye
    US_LOG_ERROR,      ///< logic and algorithm errors
    US_LOG_FATAL_ERROR ///< errors which are lead to abort
  } usLogStatus;

  /// @brief callback to process char output
  typedef void (*usLogFunc)(usLogStatus, int, const char *);
  /// @brief callback to process wide char output
  typedef void (*uswLogFunc)(usLogStatus, int, const wchar_t *);

  USENGINE_API void usSetLoggingFunc(usLogFunc func);

  typedef void * usRenderableSceneHandler;

  /// @brief begins frame in engine. Prepares container for rendering
  /// @param opts - options for frame
  /// @return handler for frame data which should be filled
  USENGINE_API usRenderableSceneHandler usBeginFrame(usVideoOptions opts);
  /// @brief wait for frame is processed and return it
  USENGINE_API usFrame usRenderFrame();
  /// @brief finishes frame and clear all resources
  USENGINE_API void usEndFrame();


  typedef struct
  {
    float x, y;
    float r, g, b;
  } usMeshObjectVertex;

  typedef struct
  {
    const usMeshObjectVertex * vertices;
    size_t count;
  } usMeshObjectConstructorArgs;

  /// @brief emplaces object in renderable scene with args passed as struct
  USENGINE_API void usRenderableScene_ConstructMeshObject(usRenderableSceneHandler handler,
                                                          usMeshObjectConstructorArgs args);

#ifdef __cplusplus
}
#endif
#endif // ENGINE_H
