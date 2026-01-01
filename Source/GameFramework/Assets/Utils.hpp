#pragma once
#include <Assets/AssetsRegistry.hpp>

namespace GameFramework::details
{
AssetType GetAssetTypeByPath(const std::filesystem::path & path) noexcept;

std::string_view AssetTypeToString(AssetType type) noexcept;
AssetType StringToAssetType(std::string_view str) noexcept;

} // namespace GameFramework::details
