#pragma once
#include <GameFramework_def.h>

#include <Assets/Asset.hpp>
#include <Files/FileStream.hpp>

namespace GameFramework
{

struct GAME_FRAMEWORK_API AssetSlot final
{
  AssetSlot(AssetType type);
  ~AssetSlot();

public:
  void SetAsset(const Uuid & uuid);
  void ClearAsset();
  const IAsset * GetAsset() const;

public:
  static size_t ReadBinary(IFileReader & stream, AssetSlot & slot);
  static void WriteBinary(IFileWriter & stream, const AssetSlot & slot);

private:
  AssetType m_assetType;
  IAsset * m_asset = nullptr;
};

} // namespace GameFramework
