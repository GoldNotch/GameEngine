#pragma once
#include <vector>

#include <GameFramework.hpp>

#include "Resource.hpp"

namespace GameFramework
{

struct StaticMeshResource : public IResource
{
  explicit StaticMeshResource(const std::filesystem::path & path);

  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const override;
  /// Upload resource to RAM
  virtual void Upload() override;
  /// Free resource from RAM
  virtual void Free() override;

private:
  struct StaticMeshTopology;
  std::vector<StaticMeshTopology> m_topologies;
};

} // namespace GameFramework
