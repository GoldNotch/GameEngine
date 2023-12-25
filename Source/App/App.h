#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#if defined(APP_STATIC_BUILD)
#define USAPP_API
#elif defined(APP_BUILD)
#define USAPP_API __declspec(dllexport)
#else
#define USAPP_API __declspec(dllimport)
#endif
#elif
#define USAPP_API
#endif

  USAPP_API void usApp_Init();
  USAPP_API void usApp_Tick();
  USAPP_API void usApp_Terminate();

#ifdef __cplusplus
}
#endif

#endif // APP_H
