#pragma once
#include <unordered_map>
#include <vector>

#include <OwnedBy.hpp>
#include <Resources/Resource.hpp>
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
  void PushObjectToDraw(IStaticMeshObject * resource);

  /// submits commands
  void RenderCache(SDL_GPURenderPass * renderPass);

  void UploadToGPU();

private:
  struct StaticMeshGpuCache final : public OwnedBy<MeshRenderer>
  {
    size_t dataHash = 0;
    SDL_GPUBuffer * vertices = nullptr;
    SDL_GPUBuffer * indices = nullptr;
    size_t num_indices = 0;
    size_t num_vertices = 0;
    MAKE_ALIAS_FOR_GET_OWNER(MeshRenderer, GetRenderer);

    explicit StaticMeshGpuCache(MeshRenderer & renderer, IStaticMeshObject * mesh);
    ~StaticMeshGpuCache();
    StaticMeshGpuCache(StaticMeshGpuCache && rhs) noexcept;
    StaticMeshGpuCache & operator=(StaticMeshGpuCache && rhs) noexcept;
  };

  std::vector<IStaticMeshObject *> m_drawCommands;
  std::unordered_map<std::filesystem::path, StaticMeshGpuCache> m_gpuCache; // GPU cache

  SDL_GPUGraphicsPipeline * m_pipeline;
  SDL_GPUShader * m_vertexShader;
  SDL_GPUShader * m_fragmentShader;

private:
  void CreatePipeline(SDL_GPUTextureFormat format);
};
} // namespace GameFramework
