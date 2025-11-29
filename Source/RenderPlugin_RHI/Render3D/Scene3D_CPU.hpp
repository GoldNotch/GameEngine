#pragma once
#include <GameFramework.hpp>
#include <Render3D/RenderData.hpp>

namespace RenderPlugin
{
struct Scene3D_GPU;

struct Scene3D_CPU final : public GameFramework::IRenderableScene3D
{
  explicit Scene3D_CPU(Scene3D_GPU & scene);
  virtual ~Scene3D_CPU() override;

  virtual void AddCube(const GameFramework::Cube & cube) override;
  virtual void SetCamera(const GameFramework::Camera & camera) override;

private:
  Scene3D_GPU * m_boundScene = nullptr;
  RenderableBatches<GameFramework::Cube> m_cubeBatches;
  GameFramework::Camera m_camera;
};
} // namespace RenderPlugin
