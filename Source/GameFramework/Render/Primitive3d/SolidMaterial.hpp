#pragma once
#include <filesystem>

#include <Render/Material.hpp>

namespace GameFramework
{

struct GAME_FRAMEWORK_API SolidMaterial : public Material
{
  static constexpr AssetType Type = AssetType::Material;
  SolidMaterial() = default;
  explicit SolidMaterial(const std::filesystem::path & shader,
                         const std::filesystem::path & texture);

public: // Material interface
  virtual std::filesystem::path GetMaterialShader() const noexcept override;

private:
  std::filesystem::path m_shader;
  std::filesystem::path m_texture;
};

} // namespace GameFramework
