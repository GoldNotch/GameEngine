#pragma once
#include <Game/Math.hpp>
#include <Render/RenderPrimitive.hpp>

namespace GameFramework
{

struct PerspectiveSettings final
{
  float fov = 45.0f;           ///< field of view in degrees
  float aspectRatio = 1.0f;    ///< height / width
  Vec2f zRange{0.01f, 100.0f}; ///< how long you can view
};

struct OrthogonalSettings final
{
  Vec2f xRange{-100.0f, 100.0f};
  Vec2f yRange{-100.0f, 100.0f};
  Vec2f zRange{-100.0f, 100.0f};
};


class GAME_FRAMEWORK_API Camera final : public IRenderPrimitive
{
public:
  Camera() = default;

  void SetPlacement(const Vec3f & pos, const Vec3f & direction,
                    const Vec3f & Up = {0.0f, 1.0f, 0.0f});
  void SetPerspectiveSettings(const PerspectiveSettings & settings);
  void SetOrthogonalSettings(const OrthogonalSettings & settings);

  Mat4f GetViewMatrix() const noexcept;
  Mat4f GetProjectionMatrix() const noexcept;
  Mat4f GetVP() const noexcept;
  //Vec3f GetRightVector() const noexcept;
  //Vec3f GetFrontVector() const noexcept;
public:
  virtual size_t Hash() const noexcept override;

private:
  bool m_isPerspective = false;
  Mat4f m_projMatrix;
  Mat4f m_viewMatrix;
};

} // namespace GameFramework
