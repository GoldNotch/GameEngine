#ifndef LAUNCHER_DEF_H
#define LAUNCHER_DEF_H

#if defined(_WIN32) || defined(__CYGWIN__)
#ifdef LAUNCHER_BUILD
#define LAUNCHER_API __declspec(dllexport)
#else
#define LAUNCHER_API __declspec(dllimport)
#endif
#elif __GNUC__ >= 4
#define LAUNCHER_API __attribute__((visibility("default")))
#else
#define LAUNCHER_API
#endif

#endif //LAUNCHER_DEF_H
