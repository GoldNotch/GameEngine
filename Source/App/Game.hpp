#pragma once
#include <vector>

#include "App.h"
#include "App.hpp"
struct TriangleObject final : public App::IAppObject
{
  virtual ~TriangleObject() = default;
  void Tick() override {}

  const glVec2 * GetVerticesData() const { return vertices; }
  const glVec3 * GetColorsData() const { return colors; }
  const size_t VerticesCount() const { return 6; }

private:
  static constexpr glVec2 vertices[] = {{-0.5f, -0.5f}, {0.5f, -0.5f}, {-0.5f, 0.5f},
                                        {-0.5f, 0.5f},  {0.5f, -0.5f}, {0.5f, 0.5f}};
  static constexpr glVec3 colors[] = {{0, 1, 1}, {1, 0, 0.5647}, {1, 1, 0},
                                      {1, 1, 0}, {1, 0, 0.5647}, {0, 0, 0}};
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

  void InitRenderableScene(const RenderSceneHandler scene)
  {
    StaticMesh mesh{};
    mesh.vertices = obj.GetVerticesData();
    mesh.vertices_count = obj.VerticesCount();
    mesh.colors = obj.GetColorsData();
    RenderScene_PushStaticMesh(scene, mesh);
  }

private:
  TriangleObject obj;
  std::vector<TriangleObject> triangles;

private:
  Game(const Game &) = delete;
  Game & operator=(const Game &) = delete;
};
