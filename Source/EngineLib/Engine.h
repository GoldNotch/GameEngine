
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

typedef unsigned int color_t;

typedef struct USENGINE_API Frame
{
	const color_t* video_frame;
	usVideoOptions opts;
}
usFrameResult;

/*
Engine is object which creates frames.
One frame is a picture and sound sample
*/

USENGINE_API bool usEngineInit(usConstructOptions options);
USENGINE_API void usBeginFrame(usVideoOptions opts);
USENGINE_API Frame usWaitForResult();
USENGINE_API void usEndFrame();
USENGINE_API void usEngineTerminate();

typedef enum
{
	INFO,
	WARNING,
	ERROR,
	FATAL_ERROR
} usLogStatus;
typedef void (*usLogFunc)(usLogStatus, int, const char*);

USENGINE_API void usSetLoggingFunc(usLogFunc func);


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