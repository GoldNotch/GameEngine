#pragma once
#include <GameFramework_def.h>

namespace GameFramework
{
// clang-format off
struct GAME_FRAMEWORK_API Vec2f { float x = 0.0f, y = 0.0f; };
struct GAME_FRAMEWORK_API Vec3f { float x = 0.0f, y = 0.0f, z = 0.0f; };
struct GAME_FRAMEWORK_API Vec4f { float x = 0.0f, y = 0.0f, z = 0.0f, w = 0.0f; };
struct GAME_FRAMEWORK_API Mat2f { float m[4] = {1.0f, 0.0f, 
                                                0.0f, 1.0f}; };
struct GAME_FRAMEWORK_API Mat3f { float m[9] = {    1.0f, 0.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f }; };
struct GAME_FRAMEWORK_API Mat4f { float m[16] = {   1.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0f, 1.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f, 1.0f };  };

struct GAME_FRAMEWORK_API Vec2d { double x = 0.0, y = 0.0; };
struct GAME_FRAMEWORK_API Vec3d { double x = 0.0, y = 0.0, z = 0.0; };
struct GAME_FRAMEWORK_API Vec4d { double x = 0.0, y = 0.0, z = 0.0, w = 0.0; };
struct GAME_FRAMEWORK_API Mat2d { double m[4] = { 1.0, 0.0, 
                                                  0.0, 1.0 }; };
struct GAME_FRAMEWORK_API Mat3d { double m[9] = {   1.0, 0.0, 0.0, 
                                                    0.0, 1.0, 0.0, 
                                                    0.0, 0.0, 1.0 }; };
struct GAME_FRAMEWORK_API Mat4d { double m[16] = {  1.0, 0.0, 0.0, 0.0,
                                                    0.0, 1.0, 0.0, 0.0,
                                                    0.0, 0.0, 1.0, 0.0,
                                                    0.0, 0.0, 0.0, 1.0}; };

inline Vec2f operator-(const Vec2f& v) { return { -v.x, -v.y }; }
inline Vec3f operator-(const Vec3f& v) { return { -v.x, -v.y, -v.z }; }
inline Vec4f operator-(const Vec4f& v) { return { -v.x, -v.y, -v.z, -v.w }; }
inline Vec2d operator-(const Vec2d& v) { return { -v.x, -v.y }; }
inline Vec3d operator-(const Vec3d& v) { return { -v.x, -v.y, -v.z }; }
inline Vec4d operator-(const Vec4d& v) { return { -v.x, -v.y, -v.z, -v.w }; }

inline Vec2f operator+(const Vec2f& v1, const Vec2f& v2) { return { v1.x + v2.x, v1.y + v2.y }; }
inline Vec3f operator+(const Vec3f& v1, const Vec3f& v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }
inline Vec4f operator+(const Vec4f& v1, const Vec4f& v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w }; }
inline Vec2d operator+(const Vec2d& v1, const Vec2d& v2) { return { v1.x + v2.x, v1.y + v2.y }; }
inline Vec3d operator+(const Vec3d& v1, const Vec3d& v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z }; }
inline Vec4d operator+(const Vec4d& v1, const Vec4d& v2) { return { v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w }; }

inline Vec2f operator*(const Vec2f& v, float m) { return { v.x * m, v.y * m }; }
inline Vec3f operator*(const Vec3f& v, float m) { return { v.x * m, v.y * m, v.z * m }; }
inline Vec4f operator*(const Vec4f& v, float m) { return { v.x * m, v.y * m, v.z * m, v.w * m }; }
inline Vec2d operator*(const Vec2d& v, double m) { return { v.x * m, v.y * m }; }
inline Vec3d operator*(const Vec3d& v, double m) { return { v.x * m, v.y * m, v.z * m }; }
inline Vec4d operator*(const Vec4d& v, double m) { return { v.x * m, v.y * m, v.z * m, v.w * m }; }

// clang-format on

inline Vec3f operator-(const Vec3f & v) noexcept
{
  return {-v.x, -v.y, -v.z};
}

} // namespace GameFramework


// Don't declare USE_GLM in header because it's only for cpp
#ifdef USE_GLM
#include <bit>
#define DECLARE_GLM_TYPE_ADAPTER(EngineType, GlmType)                                              \
  inline GlmType CastToGLM(const EngineType & type)                                                \
  {                                                                                                \
    return std::bit_cast<GlmType>(type);                                                           \
  }                                                                                                \
  inline EngineType CastFromGLM(const GlmType & type)                                              \
  {                                                                                                \
    return std::bit_cast<EngineType>(type);                                                        \
  }

#include <glm/glm.hpp>
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec2f, glm::vec2);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec3f, glm::vec3);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec4f, glm::vec4);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat2f, glm::mat2);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat3f, glm::mat3);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat4f, glm::mat4);

DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec2d, glm::dvec2);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec3d, glm::dvec3);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Vec4d, glm::dvec4);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat2d, glm::dmat2);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat3d, glm::dmat3);
DECLARE_GLM_TYPE_ADAPTER(GameFramework::Mat4d, glm::dmat4);

#endif
