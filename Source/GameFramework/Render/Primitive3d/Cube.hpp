#pragma once
#include <Game/Math.hpp>
#include <Render/Material.hpp>
#include <Render/RenderPrimitive.hpp>

namespace GameFramework
{
struct GAME_FRAMEWORK_API Cube : public IRenderPrimitive
{
  Cube();
  explicit Cube(const Vec3f & pos, const Material * material);

public:
  const Mat4f & GetTransform() const & noexcept;
  const Material* GetMaterial() const noexcept;

public:
  virtual size_t Hash() const noexcept override;

private:
  Mat4f m_transform;
  const Material * m_material = nullptr;
};
} // namespace GameFramework
