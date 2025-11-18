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
