#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <optional>

#include <Assets/Asset.hpp>
#include <Utility/Uuid.hpp>

namespace GameFramework
{

struct AssetsRegistry
{
  virtual ~AssetsRegistry() = default;

  /// @brief Add asset to the registry
  /// @param path
  /// @param asset
  /// @return
  virtual std::optional<Uuid> RegisterAsset(const std::filesystem::path & path) = 0;

  /// @brief delete asset from registry
  /// @param path
  virtual void UnregisterAsset(const Uuid & uuid) = 0;

  /// @brief uploads all meta-assets into the file in disk
  /// @param path to the file of database
  virtual void SaveDatabase(const std::filesystem::path & path) = 0;

  /// @brief loads entire database of meta-assets to RAM
  /// @param path to the file of database
  virtual void LoadDatabase(const std::filesystem::path & path) = 0;

  /// @brief get asset by uuid
  virtual const IAsset * GetAsset(const Uuid & uuid) const = 0;

  /// @brief get asset by path
  virtual const IAsset * GetAsset(const std::filesystem::path & path) const = 0;
};

using AssetsRegisryUPtr = std::unique_ptr<AssetsRegistry>;

GAME_FRAMEWORK_API AssetsRegistry & GetAssetsRegistry();

} // namespace GameFramework
