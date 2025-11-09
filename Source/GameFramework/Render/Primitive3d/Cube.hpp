#pragma once
#include <Game/Math.hpp>
#include <Render/RenderPrimitive.hpp>

namespace GameFramework
{
struct GAME_FRAMEWORK_API Cube : public IRenderPrimitive
{
  Cube();
  explicit Cube(const Vec3f & pos);

public:
  const Mat4f & GetTransform() const & noexcept;

public:
  virtual size_t Hash() const noexcept override;

private:
  Mat4f m_transform;
};
} // namespace GameFramework
