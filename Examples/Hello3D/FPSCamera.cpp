#define USE_GLM
#include "FPSCamera.hpp"

#include <glm/ext.hpp>
#include <glm/glm.hpp>

namespace
{
glm::vec3 OrthogonalVector(const glm::vec3 & v1, const glm::vec3 & v2)
{
  return glm::normalize(glm::cross(v1, v2));
}
} // namespace


namespace GameFramework
{

FPSCamera::FPSCamera(const Vec3f & up)
  : m_up(up)
{
}

void FPSCamera::RotateCamera(const Vec2f & _delta)
{
  glm::vec2 delta = CastToGLM(_delta);
  glm::vec3 forward = CastToGLM(m_direction);
  glm::vec3 up = CastToGLM(m_up);
  glm::vec3 right = -OrthogonalVector(up, forward);

  glm::vec2 offset = delta * m_sensetivity;
  glm::vec4 newCamDirection = glm::vec4(forward, 0.0);
  glm::mat4 rotYaw = glm::rotate(glm::identity<glm::mat4>(), offset.x, glm::vec3(0, 1, 0));
  newCamDirection = rotYaw * newCamDirection;
  if (glm::abs(glm::dot(forward, up) - glm::radians(89.0f)) > 0.01f)
  {
    glm::mat4 rotPitch = glm::rotate(glm::identity<glm::mat4>(), offset.y, right);
    newCamDirection = rotPitch * newCamDirection;
  }
  m_direction = CastFromGLM(glm::vec3(glm::normalize(newCamDirection)));
}

void FPSCamera::SetPosition(const Vec3f & newPos)
{
  m_position = newPos;
}


void FPSCamera::MoveCamera(const Vec3f & offset)
{
  auto newPos = CastToGLM(m_position) + CastToGLM(offset) * m_speed;
  m_position = CastFromGLM(newPos);
}

Vec3f FPSCamera::GetPosition() const noexcept
{
  return m_position;
}

Vec3f FPSCamera::GetRightVector() const noexcept
{
  return CastFromGLM(-OrthogonalVector(CastToGLM(m_up), CastToGLM(m_direction)));
}

Vec3f FPSCamera::GetFrontVector() const noexcept
{
  return m_direction;
}


} // namespace GameFramework
