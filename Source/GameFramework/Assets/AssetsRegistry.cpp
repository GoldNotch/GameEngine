#include "AssetsRegistry.hpp"

namespace GameFramework
{
class AssetsRegistryImpl : public AssetsRegistry
{
public:
  AssetsRegistryImpl() = default;


  /// @brief Add asset to the registry
  /// @param path
  /// @param asset
  /// @return
  virtual AssetUUID RegisterAsset(AssetUPtr && asset) override { return UuidNull; };

  /// @brief delete asset from registry
  /// @param path
  virtual void UnregisterAsset(AssetUUID uuid) override {};

  /// @brief uploads all meta-assets into the file in disk
  /// @param path to the file of database
  virtual void SaveDatabase(const std::filesystem::path & path) override {};

  /// @brief loads entire database of meta-assets to RAM
  /// @param path to the file of database
  virtual void LoadDatabase(const std::filesystem::path & path) override {};

  /// @brief delete the registry from RAM. Called when game is closed
  virtual void ClearRegister() override {};

private:
  virtual const Asset * GetAssetImpl(AssetUUID uuid) const override { return nullptr; };
};

GAME_FRAMEWORK_API AssetsRegistry & GetAssetsRegistry()
{
  static AssetsRegistryImpl s_assetsRegistry;
  return s_assetsRegistry;
}
} // namespace GameFramework
