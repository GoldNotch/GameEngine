#pragma once
#include <filesystem>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Resource.hpp"

namespace GameFramework
{

struct GAME_FRAMEWORK_API IStaticMeshResouce : public IResource
{
  struct StaticMeshPartDescription
  {
    glm::mat4x4 transform;
    size_t verticesCount = 0;
    size_t verticesOffset = 0;
    size_t indicesCount = 0;
    size_t indicesOffset = 0;
  };

  virtual ~IStaticMeshResouce() = default;
  virtual const std::vector<glm::vec3> & GetVertices() const & noexcept = 0;
  virtual const std::vector<uint32_t> & GetIndices() const & noexcept = 0;
  virtual const std::vector<StaticMeshPartDescription> & GetPartsDescription() const & noexcept = 0;
};

std::unique_ptr<IStaticMeshResouce> CreateStaticMeshResource(const std::filesystem::path & path);

} // namespace GameFramework
