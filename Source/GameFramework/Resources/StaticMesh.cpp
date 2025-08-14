#include "StaticMesh.hpp"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <numeric>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <GameFramework.hpp>

#include "StaticMesh.hpp"

namespace
{
glm::vec3 FromAssimpVector(const aiVector3f & v) noexcept
{
  return {v.x, v.y, v.z};
}

glm::mat4 FromAssimpMatrix(const aiMatrix4x4 & m) noexcept
{
  return {m.a1, m.a2, m.a3, m.a4, m.b1, m.b2, m.b3, m.b4,
          m.c1, m.c2, m.c3, m.c4, m.d1, m.d2, m.d3, m.d4};
}
} // namespace

// StaticMesh - is a not animated mesh object. It can contain many meshes but all of the meshes are not animated
namespace GameFramework
{

struct StaticMeshResource final : public IStaticMeshResouce
{
  friend struct StaticMeshIndexIterator;

  explicit StaticMeshResource(const std::filesystem::path & path);

  virtual const std::filesystem::path & GetPath() const & noexcept override;
  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const override;
  /// Upload resource to RAM
  virtual size_t Upload() override;
  /// Free resource from RAM
  virtual void Free() override;

public:
  virtual const std::vector<glm::vec3> & GetVertices() const & noexcept override;
  virtual const std::vector<uint32_t> & GetIndices() const & noexcept override;
  virtual const std::vector<StaticMeshPartDescription> & GetPartsDescription()
    const & noexcept override;


private:
  std::filesystem::path m_path;
  size_t m_lastUploadHash = 0;
  std::vector<glm::vec3> m_vertices;
  std::vector<uint32_t> m_indices;
  std::vector<StaticMeshPartDescription> m_partsDescription;

private:
  void ProcessMeshParts(aiNode * root,
                        const std::vector<StaticMeshPartDescription> & meshesDescription);
};

StaticMeshResource::StaticMeshResource(const std::filesystem::path & path)
  : m_path(path)
{
}

const std::filesystem::path & StaticMeshResource::GetPath() const & noexcept
{
  return m_path;
}

bool StaticMeshResource::IsUploaded() const
{
  return !m_partsDescription.empty();
}

size_t StaticMeshResource::Upload()
{
  if (IsUploaded())
    return m_lastUploadHash;

  Free();
  Assimp::Importer importer;
  const aiScene * importedScene =
    importer.ReadFile(m_path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!importedScene)
  {
    Log(LogMessageType::Error, "Failed to upload StaticMeshResource - ", GetPath());
    return 0;
  }
  m_lastUploadHash = CalcResourceTimestamp();

  std::vector<StaticMeshPartDescription> meshesDescription;
  meshesDescription.reserve(importedScene->mNumMeshes);

  for (size_t i = 0; i < importedScene->mNumMeshes; ++i)
  {
    const aiMesh * mesh = importedScene->mMeshes[i];
    const size_t verticesOffset = m_vertices.size();
    //copy vertex coordinates
    std::transform(mesh->mVertices, mesh->mVertices + mesh->mNumVertices,
                   std::back_inserter(m_vertices), ::FromAssimpVector);

    // copy indices
    size_t indicesCount = 0;
    const size_t indicesOffset = m_indices.size();
    for (size_t j = 0; j < mesh->mNumFaces; ++j)
    {
      const aiFace & face = mesh->mFaces[j];
      std::copy(face.mIndices, face.mIndices + face.mNumIndices, std::back_inserter(m_indices));
      indicesCount += face.mNumIndices;
    }

    // count vertices and indices
    meshesDescription.push_back(
      {glm::mat4x4(), mesh->mNumVertices, verticesOffset, indicesCount, indicesOffset});
  }

  ProcessMeshParts(importedScene->mRootNode, meshesDescription);

  return m_lastUploadHash;
}

void StaticMeshResource::Free()
{
  m_lastUploadHash = 0;
  m_partsDescription.clear();
  m_vertices.clear();
  m_indices.clear();
}

const std::vector<glm::vec3> & StaticMeshResource::GetVertices() const & noexcept
{
  return m_vertices;
}

const std::vector<uint32_t> & StaticMeshResource::GetIndices() const & noexcept
{
  return m_indices;
}

const std::vector<StaticMeshResource::StaticMeshPartDescription> & StaticMeshResource::
  GetPartsDescription() const & noexcept
{
  return m_partsDescription;
}

void StaticMeshResource::ProcessMeshParts(
  aiNode * root, const std::vector<StaticMeshPartDescription> & meshesDescription)
{
  for (size_t i = 0; i < root->mNumMeshes; ++i)
  {
    uint32_t meshIndex = root->mMeshes[i];
    auto && description = m_partsDescription.emplace_back(meshesDescription[meshIndex]);
    description.transform = ::FromAssimpMatrix(root->mTransformation);
  }

  for (size_t i = 0; i < root->mNumChildren; ++i)
    ProcessMeshParts(root->mChildren[i], meshesDescription);
}

std::unique_ptr<IStaticMeshResouce> CreateStaticMeshResource(const std::filesystem::path & path)
{
  return std::make_unique<StaticMeshResource>(path);
}

} // namespace GameFramework
