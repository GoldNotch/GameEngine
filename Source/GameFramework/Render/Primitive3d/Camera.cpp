#define USE_GLM
#define GLM_ENABLE_EXPERIMENTAL
#include "Camera.hpp"

#include <glm/ext.hpp>
#include <Utility/Utility.hpp>

#include "glm/gtx/hash.hpp"
namespace
{
glm::vec3 OrthogonalVector(const glm::vec3 & v1, const glm::vec3 & v2)
{
  return glm::normalize(glm::cross(v1, v2));
}
} // namespace

namespace GameFramework
{
void Camera::SetPlacement(const Vec3f & pos, const Vec3f & direction, const Vec3f & Up)
{
  glm::vec3 glmPos = CastToGLM(pos);
  glm::vec3 glmDir = CastToGLM(direction);
  glm::vec3 glmUp = CastToGLM(Up);
  glm::mat4 v = glm::lookAt(glmPos, glmPos + glmDir, glmUp);
  m_viewMatrix = CastFromGLM(v);
}

void Camera::SetPerspectiveSettings(const PerspectiveSettings & settings)
{
  glm::mat4 proj = glm::perspective(glm::radians(settings.fov), settings.aspectRatio,
                                      settings.zRange.x, settings.zRange.y);
  m_projMatrix = CastFromGLM(proj);
  m_isPerspective = true;
}

void Camera::SetOrthogonalSettings(const OrthogonalSettings & settings)
{
  glm::mat4 proj = glm::frustumZO(settings.xRange.x, settings.xRange.y, settings.yRange.x,
                                  settings.yRange.y, settings.zRange.x, settings.zRange.y);
  m_projMatrix = CastFromGLM(proj);
  m_isPerspective = false;
}

Mat4f Camera::GetViewMatrix() const noexcept
{
  return m_viewMatrix;
}

Mat4f Camera::GetProjectionMatrix() const noexcept
{
  return m_projMatrix;
}

Mat4f Camera::GetVP() const noexcept
{
  return CastFromGLM(CastToGLM(m_viewMatrix) * CastToGLM(m_projMatrix));
}

size_t Camera::Hash() const noexcept
{
  size_t hash = 0;
  Utils::hash_combine(hash, CastToGLM(m_viewMatrix), CastToGLM(m_projMatrix));
  return hash;
}

//Vec3f Camera::GetRightVector() const noexcept
//{
//  return -OrthogonalVector(m_up, m_direction);
//}
//
//Vec3f Camera::GetFrontVector() const noexcept
//{
//  CastToGLM(m_viewMatrix)
//  return m_direction;
//}
} // namespace GameFramework
