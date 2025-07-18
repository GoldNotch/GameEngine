#include "StaticMesh.hpp"

#include <filesystem>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

// StaticMesh - is a not animated mesh object. It can contain many meshes but all of the meshes are not animated
namespace GameFramework
{

struct StaticMeshResource::StaticMeshTopology
{
  explicit StaticMeshTopology(const aiMesh & mesh);

  std::vector<vec3> m_vertices;
  std::vector<vec3> m_normals;
  std::vector<vec3> m_tangets;
  std::vector<uint32_t> m_indices;
};

StaticMeshResource::StaticMeshTopology::StaticMeshTopology(const aiMesh & mesh)
{
}

StaticMeshResource::StaticMeshResource(const std::filesystem::path & path)
  : IResource(path)
{
}

bool StaticMeshResource::IsUploaded() const
{
  return !m_topologies.empty();
}

void StaticMeshResource::Upload()
{
  Assimp::Importer importer;

  const aiScene * scene =
    importer.ReadFile(GetPath().string(), aiProcess_Triangulate | aiProcess_FlipUVs);

  {
    Log(LogMessageType::Error, "Failed to upload StaticMeshResource - ", GetPath());
    return;
  }
}

void StaticMeshResource::Free()
{
  m_topologies.clear();
}


} // namespace GameFramework
