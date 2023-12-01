
#ifndef ENGINE_H
#define ENGINE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#	if defined(ENGINE_STATIC_BUILD) || defined(APP_IMPLEMENTATION)
#		define USENGINE_API
#	elif  defined(ENGINE_BUILD)
#		define USENGINE_API __declspec(dllexport)
#	else
#		define USENGINE_API __declspec(dllimport)
#	endif
#elif
#    define USENGINE_API
#endif

#ifdef _WIN32
#	if  defined(APP_BUILD)
#		define USAPP_API __declspec(dllimport)
#	else
#		define USAPP_API __declspec(dllexport)
#	endif
#elif
#    define USAPP_API
#endif

// ------------------------- Engine API --------------------------

struct USENGINE_API USEngine;
typedef  struct USEngine* USEnginePtr;

/// Options for frame's picture (width, height resolution, maybe channels, etc)
typedef struct USENGINE_API VideoOpts
{
	int width, height;
}
usVideoOptions;

typedef struct USENGINE_API ConstructOpts
{
	usVideoOptions video_opts; ///< options for video's picture
}
usConstructOptions;

typedef int pixel_t;

typedef struct USENGINE_API Frame
{
	const pixel_t* video_frame;
}
usFrameResult;

/*
Engine is object which creates frames.
One frame is a picture and sound sample
*/

USENGINE_API USEnginePtr usEngine_Create(usConstructOptions options);
USENGINE_API void usBeginFrame();
USENGINE_API void usWaitForResult();
USENGINE_API void usEndFrame();
USENGINE_API void usEngine_Destroy(USEnginePtr engine);


// ---------------------------- App API ---------------------------

struct USAPP_API USApp;
typedef struct USApp* USAppPtr;

USAPP_API USAppPtr usApp_Create();
USAPP_API void usApp_Tick(USAppPtr app);
USAPP_API void usApp_Destroy(USAppPtr app);

#ifdef __cplusplus
}
#endif
#endif // ENGINE_H