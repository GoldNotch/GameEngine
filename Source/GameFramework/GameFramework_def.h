#ifndef GAME_FRAMEWORK_DEF_H
#define GAME_FRAMEWORK_DEF_H


#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef GAME_FRAMEWORK_BUILD
#define GAME_API __declspec(dllimport)
#define GAME_FRAMEWORK_API __declspec(dllexport)
#else
#define GAME_API __declspec(dllexport)
#define GAME_FRAMEWORK_API __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
#define GAME_API __attribute__((visibility("default")))
#define GAME_FRAMEWORK_API __attribute__((visibility("default")))
#else
#define GAME_API
#define GAME_FRAMEWORK_API
#endif

#endif // GAME_FRAMEWORK_DEF_H
