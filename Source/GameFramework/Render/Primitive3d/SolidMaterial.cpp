#include "SolidMaterial.hpp"

namespace GameFramework
{
SolidMaterial::SolidMaterial(const std::filesystem::path & shader,
                             const std::filesystem::path & texture)
  : m_shader(shader)
  , m_texture(texture)
{
}

std::filesystem::path SolidMaterial::GetMaterialShader() const noexcept
{
  return m_shader;
}

} // namespace GameFramework
