#define USE_GLM
#define GLM_ENABLE_EXPERIMENTAL
#include "Cube.hpp"

#include "glm/gtx/hash.hpp"

namespace GameFramework
{

Cube::Cube()
{
}

Cube::Cube(const Vec3f & pos)
{
  glm::mat4 m = glm::identity<glm::mat4>();
  m = glm::translate(m, CastToGLM(pos));
  m_transform = CastFromGLM(m);
}

const Mat4f & Cube::GetTransform() const & noexcept
{
  return m_transform;
}

size_t Cube::Hash() const noexcept
{
  return std::hash<glm::mat4>{}(CastToGLM(m_transform));
}

} // namespace GameFramework
