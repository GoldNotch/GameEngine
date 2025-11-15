#pragma once
#include <memory>

#include <Render/Primitive3d/Camera.hpp>
#include <Render/Primitive3d/Cube.hpp>

namespace GameFramework
{
struct IRenderableScene3D
{
  virtual ~IRenderableScene3D() = default;
  virtual void AddCube(const Cube & cube) = 0;
  virtual void SetCamera(const Camera & camera) = 0;
};

using Scene3DUPtr = std::unique_ptr<IRenderableScene3D>;
} // namespace GameFramework
