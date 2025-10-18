#ifndef GAME_SYSTEMS_DEF_H
#define GAME_SYSTEMS_DEF_H


#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef GAME_SYSTEMS_BUILD
#define GAME_SYSTEMS_API __declspec(dllexport)
#else
#define GAME_SYSTEMS_API __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
#define GAME_SYSTEMS_API __attribute__((visibility("default")))
#else
#define GAME_SYSTEMS_API
#endif

#endif // GAME_SYSTEMS_DEF_H
