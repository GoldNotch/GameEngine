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

struct MeshRenderer final : public OwnedBy<DrawTool_SDL>
{
  explicit MeshRenderer(DrawTool_SDL & drawTool, SDL_GPUTextureFormat format);
  ~MeshRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(DrawTool_SDL, GetDrawTool);

  /// add object to draw cache
  void PushObjectToDraw(IStaticMeshResouce * resource);

  /// submits commands
  void RenderCache(SDL_GPURenderPass * renderPass);

  void UploadToGPU();

private:
  struct StaticMeshGpuCache final : public OwnedBy<MeshRenderer>
  {
    size_t dataHash = 0;
    SDL_GPUBuffer * vertices = nullptr;
    SDL_GPUBuffer * indices = nullptr;
    std::vector<IStaticMeshResouce::StaticMeshPartDescription> commands;
    MAKE_ALIAS_FOR_GET_OWNER(MeshRenderer, GetRenderer);

    explicit StaticMeshGpuCache(MeshRenderer & renderer, IStaticMeshResouce * mesh, size_t dataHash);
    ~StaticMeshGpuCache();
    StaticMeshGpuCache(StaticMeshGpuCache && rhs) noexcept;
    StaticMeshGpuCache & operator=(StaticMeshGpuCache && rhs) noexcept;
  };

  std::vector<IStaticMeshResouce *> m_drawCommands;
  std::unordered_map<std::filesystem::path, StaticMeshGpuCache> m_gpuCache; // GPU cache

  SDL_GPUGraphicsPipeline * m_pipeline;
  SDL_GPUShader * m_vertexShader;
  SDL_GPUShader * m_fragmentShader;

private:
  void CreatePipeline(SDL_GPUTextureFormat format);
};
} // namespace GameFramework
