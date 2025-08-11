#pragma once
#include <filesystem>
#include <functional>
#include <memory>

#include "Resource.hpp"

namespace GameFramework
{

struct GAME_FRAMEWORK_API IStaticMeshResouce : public IResource
{
  virtual ~IStaticMeshResouce() = default;
  virtual size_t GetVerticesCount() const noexcept = 0;
  virtual size_t GetIndicesCount() const noexcept = 0;
  using MeshNodesTraverseFunc = std::function<void(size_t, size_t, size_t, size_t)>;

  virtual void ForEachMeshNode(MeshNodesTraverseFunc && processNode) = 0;
  virtual void ForEachVertex(std::function<void(float (*)[3])>) = 0;
  virtual void ForEachIndex(std::function<void(int)>) = 0;
};

std::unique_ptr<IStaticMeshResouce> CreateStaticMeshResource(const std::filesystem::path & path);

} // namespace GameFramework
