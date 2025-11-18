#pragma once
#include <Game/Math.hpp>

namespace GameFramework
{
struct FPSCamera final
{
public:
  explicit FPSCamera(const Vec3f & up);
  void RotateCamera(const Vec2f & delta);
  void SetPosition(const Vec3f& newPos);
  void MoveCamera(const Vec3f & offset);
  void SetSpeed(float speed) { m_speed = speed; }
  void SetSensetivity(float sensetivity) { m_sensetivity = sensetivity; }

  Vec3f GetPosition() const noexcept;
  Vec3f GetRightVector() const noexcept;
  Vec3f GetFrontVector() const noexcept;

private:
  const Vec3f m_up;
  float m_speed = 0.05f;
  float m_sensetivity = 0.01f;

  Vec3f m_position{0.0f};
  Vec3f m_direction{0.0, 0.0, -1.0f};
};

} // namespace GameFramework
