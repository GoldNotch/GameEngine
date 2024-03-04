#ifndef SCENE_H
#define SCENE_H

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef RENDERING_API

#ifdef _WIN32
#if defined(RENDERING_STATIC_BUILD)
#define RENDERING_API
#elif defined(RENDERING_BUILD)
#define RENDERING_API __declspec(dllexport)
#else
#define RENDERING_API __declspec(dllimport)
#endif

#else
#define RENDERING_API
#endif

#endif


  typedef struct
  {
    float x, y;
  } glVec2;

  typedef struct
  {
    float x, y, z;
  } glVec3;

  typedef struct
  {
    float x, y, z, w;
  } glVec4;


  typedef struct StaticMeshData
  {
    size_t vertices_count;
    const glVec2 * vertices;
    const glVec3 * colors;

    size_t indices_count;
    const unsigned int * indices;
  } StaticMesh;

  //----------------- Scene API ------------------------

  typedef void * RenderSceneHandler;

  /// @brief gets render scene container
  RENDERING_API RenderSceneHandler AcquireRenderScene();

  /// @brief pushes mesh data into scene
  RENDERING_API void RenderScene_PushStaticMesh(RenderSceneHandler scene, StaticMesh mesh);


#ifdef __cplusplus
}
#endif

#endif
