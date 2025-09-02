#pragma once
#include <unordered_map>
#include <vector>

#include <OwnedBy.hpp>
#include <Resources/StaticMesh.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct DrawTool_SDL;
class StaticMeshResource;
struct RenderTarget;

struct MeshRenderer final : public OwnedBy<DrawTool_SDL>
{
  explicit MeshRenderer(DrawTool_SDL & drawTool);
  ~MeshRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(DrawTool_SDL, GetDrawTool);

  /// add object to draw cache
  void PushObjectToDraw(IStaticMeshResource * resource);

  /// submits commands
  void RenderCache(const RenderTarget & renderTarget);
  void RebuildPipeline(const RenderTarget & renderTarget);

  void UploadToGPU();

private:
  struct StaticMeshGpuCache final : public OwnedBy<MeshRenderer>
  {
    size_t dataHash = 0;
    SDL_GPUBuffer * vertices = nullptr;
    SDL_GPUBuffer * indices = nullptr;
    std::vector<IStaticMeshResource::StaticMeshPartDescription> commands;
    MAKE_ALIAS_FOR_GET_OWNER(MeshRenderer, GetRenderer);

    explicit StaticMeshGpuCache(MeshRenderer & renderer, IStaticMeshResource * mesh,
                                size_t dataHash);
    ~StaticMeshGpuCache();
    StaticMeshGpuCache(StaticMeshGpuCache && rhs) noexcept;
    StaticMeshGpuCache & operator=(StaticMeshGpuCache && rhs) noexcept;
  };

  std::vector<IStaticMeshResource *> m_drawCommands;
  std::unordered_map<std::filesystem::path, StaticMeshGpuCache> m_gpuCache; // GPU cache

  SDL_GPUGraphicsPipeline * m_pipeline = nullptr;
  SDL_GPUShader * m_vertexShader;
  SDL_GPUShader * m_fragmentShader;
};
} // namespace GameFramework
