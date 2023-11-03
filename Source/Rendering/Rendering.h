#ifndef RENDERING_H
#define RENDERING_H

#ifdef _WIN32
#ifdef BUILD_DLL
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif
#else
#define DLL_API
#endif



#endif // RENDERING_H