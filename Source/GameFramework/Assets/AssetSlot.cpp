#include "AssetSlot.hpp"

#include <Assets/AssetsRegistry.hpp>

namespace GameFramework
{
AssetSlot::AssetSlot(AssetType type)
  : m_assetType(type)
{
}

AssetSlot::~AssetSlot()
{
  ClearAsset();
}

void AssetSlot::SetAsset(const Uuid & uuid)
{
  const IAsset * foundAsset = GetAssetsRegistry().GetAsset(uuid);
  if (foundAsset && foundAsset->GetType() == m_assetType)
  {
    m_asset = const_cast<IAsset *>(foundAsset);
    if (m_asset)
      m_asset->AddUser();
  }
}

void AssetSlot::ClearAsset()
{
  if (m_asset)
    m_asset->ReleaseUser();
  m_asset = nullptr;
}

const IAsset * AssetSlot::GetAsset() const
{
  return m_asset;
}

size_t AssetSlot::ReadBinary(IFileReader & stream, AssetSlot & slot)
{
  Uuid assetUuid;
  size_t result = stream.ReadValue(assetUuid);
  slot.SetAsset(assetUuid);
  return result;
}

void AssetSlot::WriteBinary(IFileWriter & stream, const AssetSlot & slot)
{
  stream.WriteValue(slot.m_asset->GetUUID());
}

} // namespace GameFramework
