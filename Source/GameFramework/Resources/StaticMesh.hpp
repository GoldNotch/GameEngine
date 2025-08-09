#pragma once
#include <vector>

#include <GameFramework.hpp>

#include "Resource.hpp"

namespace GameFramework
{

struct StaticMeshResource final : public IResource
{
  explicit StaticMeshResource(const std::filesystem::path & path);
  ~StaticMeshResource();

  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const override;
  /// Upload resource to RAM
  virtual size_t Upload() override;
  /// Free resource from RAM
  virtual void Free() override;

public:
  size_t GetVerticesCount() const;
  size_t GetIndicesCount() const;


private:
  struct PrivateData;
  std::unique_ptr<PrivateData> m_privateData = nullptr;
  size_t m_lastUploadHash = 0;
};

} // namespace GameFramework
