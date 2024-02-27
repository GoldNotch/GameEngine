#pragma once
#include <vector>

#include "App.hpp"
struct TriangleObject final : public App::IAppObject
{
  virtual ~TriangleObject() = default;
  void Tick() override {}

  const float * GetVerticesData() const { return vertices; }
  const size_t VerticesCount() const { return 3; }

private:
  static constexpr float vertices[] = {-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.5f, -0.5f, 0.0f,
                                       1.0f,  0.0f,  0.0f, 0.5f, 1.0,  0.0f, 0.0f};
};

struct Game final
{
public:
  Game() { triangles.emplace_back(); }
  ~Game() = default;

  void Tick()
  {
    for (auto && triangle : triangles)
      triangle.Tick();
  }

  void InitRenderableScene(void * const scene_handler)
  {
    //TODO:pass data from trianlges to scene
    //for (auto && tri : triangles)
    //{
    //  //usMeshObjectConstructorArgs args{reinterpret_cast<const usMeshObjectVertex *>(
    //  //                                   tri.GetVerticesData()),
    //  //                                 tri.VerticesCount()};
    //  //usRenderableScene_ConstructMeshObject(scene_handler, args);
    //}
  }

private:
  std::vector<TriangleObject> triangles;

private:
  Game(const Game &) = delete;
  Game & operator=(const Game &) = delete;
};
