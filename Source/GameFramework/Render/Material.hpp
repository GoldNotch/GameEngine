#pragma once
#include <filesystem>
#include <memory>
#include <string>

#include <Assets/Asset.hpp>

namespace GameFramework
{
/// @brief describes how object should be drawn.
// TODO: probably there must be multiple types of material
struct Material : public Asset
{
  virtual ~Material() = default;
  virtual std::filesystem::path GetMaterialShader() const noexcept = 0;
  //virtual std::vector<std::filesystem::path> GetTextures() const noexcept = 0;
};

using MaterialUPtr = std::unique_ptr<Material>;

} // namespace GameFramework
