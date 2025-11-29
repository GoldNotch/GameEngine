#pragma once
#include <array>
#include <unordered_map>
#include <vector>

#include <GameFramework.hpp>

namespace RenderPlugin
{
template<typename ObjT>
using RenderBatch = std::pair<size_t /*hash*/, std::vector<ObjT>>;
template<typename ObjT>
using RenderableBatches =
  std::unordered_map<std::filesystem::path /*shaderPath*/, RenderBatch<ObjT>>;

struct ViewProjection final
{
  GameFramework::Mat4f view;
  GameFramework::Mat4f projection;
};

using TextureSlots = std::array<int, 16>;
static constexpr int NullTexture = -1;
static constexpr TextureSlots UnusedSlots = {NullTexture, NullTexture, NullTexture, NullTexture,
                                             NullTexture, NullTexture, NullTexture, NullTexture,
                                             NullTexture, NullTexture, NullTexture, NullTexture,
                                             NullTexture, NullTexture, NullTexture, NullTexture};
} // namespace RenderPlugin
