#include "StaticMesh.hpp"

#include <chrono>
#include <filesystem>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h>       // Output data structure
#include <GameFramework.hpp>

// StaticMesh - is a not animated mesh object. It can contain many meshes but all of the meshes are not animated
namespace GameFramework
{

struct StaticMeshResource final : public IStaticMeshObject,
                                  public IResource
{
  explicit StaticMeshResource(const std::filesystem::path & path);

  virtual const std::filesystem::path & GetPath() const & noexcept override;
  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const override;
  /// Upload resource to RAM
  virtual size_t Upload() override;
  /// Free resource from RAM
  virtual void Free() override;

public:
  virtual size_t GetVerticesCount() const noexcept override;
  virtual size_t GetIndicesCount() const noexcept override;


private:
  std::filesystem::path m_path;
  size_t m_lastUploadHash = 0;
  std::unique_ptr<Assimp::Importer> m_importer = nullptr;
  const aiScene * m_importedScene = nullptr;
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
  return !m_importedScene;
}

size_t StaticMeshResource::Upload()
{
  if (IsUploaded())
    return m_lastUploadHash;

  Free();
  m_importer = std::make_unique<Assimp::Importer>(GetPath());
  m_importedScene =
    m_importer->ReadFile(m_path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!m_importedScene)
  {
    Log(LogMessageType::Error, "Failed to upload StaticMeshResource - ", GetPath());
    return 0;
  }
  m_lastUploadHash = CalcResourceTimestamp();

  return m_lastUploadHash;
}

void StaticMeshResource::Free()
{
  m_importer.reset();
  m_importedScene = nullptr;
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


std::unique_ptr<IStaticMeshObject> CreateStaticMeshObject(const std::filesystem::path & path)
{
  return std::make_unique<StaticMeshResource>(path);
}

} // namespace GameFramework
