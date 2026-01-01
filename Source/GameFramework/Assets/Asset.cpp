#include "Asset.hpp"

#include <Assets/Utils.hpp>

namespace GameFramework
{

struct AssetImpl : public IAsset
{
  explicit AssetImpl(const std::filesystem::path & path);
  explicit AssetImpl(const std::filesystem::path & path, const Uuid & uuid, AssetType type);

  virtual Uuid GetUUID() const noexcept override { return m_uuid; }
  virtual std::filesystem::path GetPath() const noexcept override { return m_path; }
  virtual AssetType GetType() const noexcept override { return m_type; }
  virtual void AddUser() override { m_refCounter++; }
  virtual void ReleaseUser() override { m_refCounter--; }
  virtual size_t GetUsersCount() const noexcept override { return m_refCounter; }

private:
  std::filesystem::path m_path;
  Uuid m_uuid;
  AssetType m_type;
  std::atomic_size_t m_refCounter = 0;
  //probably should contain a list of users - another game objects which are use this asset
};

AssetImpl::AssetImpl(const std::filesystem::path & path)
  : m_path(path)
  , m_type(details::GetAssetTypeByPath(path))
  , m_uuid(Uuid::MakeRandomUuid())
{
}

AssetImpl::AssetImpl(const std::filesystem::path & path, const Uuid & uuid, AssetType type)
  : m_path(path)
  , m_uuid(uuid)
  , m_type(type)
{
}
} // namespace GameFramework

namespace GameFramework::details
{

AssetUPtr CreateAsset(const std::filesystem::path & path)
{
  return std::make_unique<AssetImpl>(path);
}

AssetUPtr FillAsset(const Uuid & uuid, AssetType type, const std::filesystem::path & path)
{
  return std::make_unique<AssetImpl>(path, uuid, type);
}
} // namespace GameFramework::details
