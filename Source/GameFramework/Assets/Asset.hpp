#pragma once
#include <GameFramework_def.h>

#include <filesystem>
#include <memory>

#include <Utility/Uuid.hpp>

namespace GameFramework
{
using AssetUUID = UUID;

enum class AssetType : uint8_t
{
  Unknown,
  Material
};

/// @brief Asset is a meta data of game's resource
struct GAME_FRAMEWORK_API Asset
{
  static constexpr AssetType Type = AssetType::Unknown;
  Asset();
  virtual ~Asset() = default;
  inline AssetUUID GetUUID() const noexcept { return m_uuid; }
  inline AssetType GetType() const noexcept { return Type; }

private:
  AssetUUID m_uuid;

private:
  Asset(const Asset &) = delete;
  Asset & operator=(const Asset &) = delete;
};

using AssetUPtr = std::unique_ptr<Asset>;
} // namespace GameFramework
