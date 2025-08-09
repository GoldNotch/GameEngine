#include "StaticMesh.hpp"

#include <chrono>
#include <filesystem>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure

#include "StaticMesh.hpp"

// StaticMesh - is a not animated mesh object. It can contain many meshes but all of the meshes are not animated
namespace GameFramework
{

struct StaticMeshResource::PrivateData
{
  const aiScene * scene = nullptr;

  explicit PrivateData(const std::filesystem::path & path)
  {
    scene = m_importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);
  }

private:
  Assimp::Importer m_importer;
};

StaticMeshResource::StaticMeshResource(const std::filesystem::path & path)
  : IResource(path)
{
}

StaticMeshResource::~StaticMeshResource() = default;

bool StaticMeshResource::IsUploaded() const
{
  return !m_privateData;
}

size_t StaticMeshResource::Upload()
{
  if (IsUploaded())
    return m_lastUploadHash;

  m_privateData = std::make_unique<PrivateData>(GetPath());
  m_lastUploadHash = CalcResourceTimestamp();
  {
    Log(LogMessageType::Error, "Failed to upload StaticMeshResource - ", GetPath());
    return 0;
  }

  return m_lastUploadHash;
}

void StaticMeshResource::Free()
{
  m_privateData.reset();
  m_lastUploadHash = 0;
}

size_t StaticMeshResource::GetVerticesCount() const
{
  return size_t();
}

size_t StaticMeshResource::GetIndicesCount() const
{
  return size_t();
}


} // namespace GameFramework
