#pragma once

#include <filesystem>

#include <Utility/Uuid.hpp>

namespace GameFramework
{
enum class AssetType
{
  Unknown,
  Text,          //.txt, .md
  Config,        //.json, .ini,
  Picture,       // .bmp, .png, jpg, dds, tga
  Audio,         // wav, mp3, ogg,
  ShaderSource,  // glsl, hlsl, vert, frag, geom
  ShaderBinary,  // spv, dxil, cso
  ShaderInclude, // inc, hlsli, glsli
};

struct IAsset
{
  virtual ~IAsset() = default;
  virtual Uuid GetUUID() const noexcept = 0;
  virtual std::filesystem::path GetPath() const noexcept = 0;
  virtual AssetType GetType() const noexcept = 0;
  virtual void AddUser() = 0;
  virtual void ReleaseUser() = 0;
  virtual size_t GetUsersCount() const noexcept = 0;
};
using AssetUPtr = std::unique_ptr<IAsset>;

namespace details
{
AssetUPtr CreateAsset(const std::filesystem::path & path);
AssetUPtr FillAsset(const Uuid & uuid, AssetType type, const std::filesystem::path & path);

} // namespace details
} // namespace GameFramework
