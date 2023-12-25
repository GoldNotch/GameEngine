
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

  // ------------------------- Engine API --------------------------

  /// @brief Options for frame's picture (width, height resolution, maybe channels, etc)
  typedef struct USENGINE_API VideoOpts
  {
    int width, height;
  } usVideoOptions;

  /// @brief max count of used gpus in app
  const size_t MaxRequiredGPUs = 1;

  /// @brief options for rendering system initializing
  typedef struct USENGINE_API RenderingOpts
  {
    bool gpu_autodetect;
    size_t required_gpus;
    const char * gpu_name[MaxRequiredGPUs];
  } usRenderingOptions;

  /// @brief options for engine constructing
  typedef struct USENGINE_API ConstructOpts
  {
    usRenderingOptions rendering_options;
  } usConstructOptions;

  typedef unsigned int color_t;

  /// @brief result of one frame returned from engine
  typedef struct USENGINE_API Frame
  {
    const color_t * video_frame;
    usVideoOptions opts;
  } usFrameResult;

  /*
Engine is object which creates frames.
One frame is a picture and sound sample
*/

  /// @brief initialize engine
  /// @param options for creation
  /// @return true if success
  USENGINE_API bool usEngineInit(usConstructOptions options);

  /// @brief begins frame in engine
  /// @param opts - options for frame
  /// @return
  USENGINE_API void usBeginFrame(usVideoOptions opts);
  /// @brief wait for frame is processed and return it
  /// @return FrameData
  USENGINE_API Frame usWaitForResult();
  /// @brief finishes frame and clear all resources
  /// @return
  USENGINE_API void usEndFrame();
  /// @brief destroy engine and clear all resources
  /// @return
  USENGINE_API void usEngineTerminate();

  typedef enum
  {
    US_LOG_INFO,
    US_LOG_WARNING,
    US_LOG_ERROR,
    US_LOG_FATAL_ERROR
  } usLogStatus;
  typedef void (*usLogFunc)(usLogStatus, int, const char *);

  USENGINE_API void usSetLoggingFunc(usLogFunc func);

#ifdef __cplusplus
}
#endif
#endif // ENGINE_H
