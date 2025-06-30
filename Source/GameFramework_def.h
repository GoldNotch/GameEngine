#pragma once

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef GAME_FRAMEWORK_BUILD
#define GAME_API __declspec(dllimport)
#else
#define GAME_API __declspec(dllexport)
#endif
#elif __GNUC__ >= 4
#define GAME_API __attribute__ ((visibility ("default")))
#else
#define GAME_API
#endif