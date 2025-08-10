#pragma once
#include <filesystem>
#include <memory>

#include "Resource.hpp"

namespace GameFramework
{

struct GAME_FRAMEWORK_API IStaticMeshObject : public IResource
{
  virtual ~IStaticMeshObject() = default;
  virtual size_t GetVerticesCount() const noexcept = 0;
  virtual size_t GetIndicesCount() const noexcept = 0;
};

std::unique_ptr<IStaticMeshObject> CreateStaticMeshObject(const std::filesystem::path & path);

} // namespace GameFramework
