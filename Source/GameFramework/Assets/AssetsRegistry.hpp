#pragma once
#include <GameFramework_def.h>

#include <filesystem>

#include <Assets/Asset.hpp>

namespace GameFramework
{
struct AssetsRegistry
{
  virtual ~AssetsRegistry() = default;

  /// @brief Add asset to the registry
  /// @param path
  /// @param asset
  /// @return
  virtual AssetUUID RegisterAsset(AssetUPtr && asset) = 0;

  /// @brief delete asset from registry
  /// @param path
  virtual void UnregisterAsset(AssetUUID uuid) = 0;

  /// @brief uploads all meta-assets into the file in disk
  /// @param path to the file of database
  virtual void SaveDatabase(const std::filesystem::path & path) = 0;

  /// @brief loads entire database of meta-assets to RAM
  /// @param path to the file of database
  virtual void LoadDatabase(const std::filesystem::path & path) = 0;

  /// @brief delete the registry from RAM. Called when game is closed
  virtual void ClearRegister() = 0;

  template<typename T>
  const T * GetAsset(AssetUUID uuid) const
  {
    const Asset * asset = GetAssetImpl(uuid);
#ifdef _DEBUG
    return dynamic_cast<const T *>(asset);
#else
    if (asset->GetType() != T::Type)
    {
      return nullptr;
    }
    return static_cast<const T *>(asset);
#endif
  }

protected:
  virtual const Asset * GetAssetImpl(AssetUUID uuid) const = 0;
};

/// @brief get assets registry
GAME_FRAMEWORK_API AssetsRegistry & GetAssetsRegistry();

} // namespace GameFramework
