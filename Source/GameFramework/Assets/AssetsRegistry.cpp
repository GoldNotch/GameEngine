#include "AssetsRegistry.hpp"

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <vector>

#include <Assets/Asset.hpp>
#include <Assets/Utils.hpp>
#include <Files/FileManager.hpp>
#include <Utility/StringUtils.hpp>

namespace GameFramework
{

struct AssetsRegistryImpl : public AssetsRegistry
{
  AssetsRegistryImpl() = default;
  virtual ~AssetsRegistryImpl() override = default;

  /// @brief Add asset to the registry
  /// @param path
  /// @param asset
  /// @return
  virtual std::optional<Uuid> RegisterAsset(const std::filesystem::path & path) override;

  /// @brief delete asset from registry
  /// @param path
  virtual void UnregisterAsset(const Uuid & uuid) override;

  /// @brief uploads all meta-assets into the file in disk
  /// @param path to the file of database
  virtual void SaveDatabase(const std::filesystem::path & path) override;

  /// @brief loads entire database of meta-assets to RAM
  /// @param path to the file of database
  virtual void LoadDatabase(const std::filesystem::path & path) override;

  /// @brief get asset by uuid
  virtual const IAsset * GetAsset(const Uuid & uuid) const override;

  /// @brief get asset by path
  virtual const IAsset * GetAsset(const std::filesystem::path & path) const override;

private:
  using AssetsContainer = std::vector<AssetUPtr>;
  AssetsContainer m_assets;
  std::unordered_map<Uuid, size_t /*index*/> m_assetsByUuids;
  std::unordered_map<std::filesystem::path, size_t /*index*/> m_assetsByPath;
};

std::optional<Uuid> AssetsRegistryImpl::RegisterAsset(const std::filesystem::path & path)
{
  // check that asset with the path exists
  if (auto it = m_assetsByPath.find(path); it != m_assetsByPath.end())
    return m_assets[it->second]->GetUUID();

  // check if file exists
  if (!std::filesystem::exists(path))
    return std::nullopt;

  auto && asset = m_assets.emplace_back(details::CreateAsset(path));
  assert(!m_assetsByUuids.contains(asset->GetUUID()));
  m_assetsByPath[path] = m_assets.size() - 1;
  m_assetsByUuids[asset->GetUUID()] = m_assets.size() - 1;
  return asset->GetUUID();
}

void AssetsRegistryImpl::UnregisterAsset(const Uuid & uuid)
{
  auto it = m_assetsByUuids.find(uuid);
  if (it == m_assetsByUuids.end())
    return;
  size_t removingIndex = it->second;

  m_assetsByUuids.erase(uuid);
  m_assetsByPath.erase(m_assets[removingIndex]->GetPath());

  auto && lastAsset = m_assets.back();
  {
    auto it = m_assetsByUuids.find(lastAsset->GetUUID());
    it->second = removingIndex;
  }
  {
    auto it = m_assetsByPath.find(lastAsset->GetPath());
    it->second = removingIndex;
  }

  std::swap(m_assets.back(), m_assets[removingIndex]);
  m_assets.pop_back();
}

void AssetsRegistryImpl::SaveDatabase(const std::filesystem::path & path)
{
  // save to csv
  constexpr char delimiter = ';';
  constexpr std::string_view header = "uuid;type;path\n";

  if (FileWriterUPtr stream = GetFileManager().OpenWrite(path))
  {
    stream->WriteValue(header);
    for (auto && asset : m_assets)
    {
      stream->WriteValue(asset->GetUUID().ToString());
      stream->WriteValue(delimiter);
      stream->WriteValue(details::AssetTypeToString(asset->GetType()));
      stream->WriteValue(delimiter);
      stream->WriteValue(asset->GetPath().wstring());
      stream->WriteValue('\n');
    }
    stream->Flush();
  }
}

void AssetsRegistryImpl::LoadDatabase(const std::filesystem::path & path)
{
  if (FileReaderUPtr reader = GetFileManager().OpenRead(path))
  {
    std::vector<AssetUPtr> newAssets;
    std::unordered_map<Uuid, size_t> assetsByUuid;
    std::unordered_map<std::filesystem::path, size_t> assetsByPath;

    std::string header;
    reader->ReadLine(header);

    std::string line;
    while (size_t readSymbols = reader->ReadLine(line))
    {
      std::vector<std::string_view> data = Utils::Split(line, ';');
      std::optional<Uuid> uuid = Uuid::MakeFromString(data[0]);
      AssetType type = details::StringToAssetType(data[1]);
      std::filesystem::path path = data[2];

      if (uuid.has_value() && type != AssetType::Unknown)
      {
        auto asset = details::FillAsset(*uuid, type, path);
        newAssets.push_back(std::move(asset));
        assetsByPath.insert({path, newAssets.size() - 1});
        assetsByUuid.insert({*uuid, newAssets.size() - 1});
      }

      line.clear();
    }

    m_assets = std::move(newAssets);
    m_assetsByUuids = std::move(assetsByUuid);
    m_assetsByPath = std::move(assetsByPath);
  }
}

const IAsset * AssetsRegistryImpl::GetAsset(const Uuid & uuid) const
{
  auto it = m_assetsByUuids.find(uuid);
  if (it == m_assetsByUuids.end())
    return nullptr;

  return m_assets[it->second].get();
}


const IAsset * AssetsRegistryImpl::GetAsset(const std::filesystem::path & path) const
{
  auto it = m_assetsByPath.find(path);
  if (it == m_assetsByPath.end())
    return nullptr;

  return m_assets[it->second].get();
}

} // namespace GameFramework

namespace GameFramework
{

GAME_FRAMEWORK_API AssetsRegistry & GetAssetsRegistry()
{
  static AssetsRegistryImpl s_instance;
  return s_instance;
}


} // namespace GameFramework
