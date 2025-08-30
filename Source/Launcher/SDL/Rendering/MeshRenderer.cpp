#include "MeshRenderer.hpp"

#include <Resources/StaticMesh.hpp>
#include <SDL/Rendering/ConnectionGPU.hpp>

#include "DrawTool.hpp"

namespace GameFramework
{

MeshRenderer::MeshRenderer(DrawTool_SDL & drawTool, SDL_GPUTextureFormat format)
  : OwnedBy<DrawTool_SDL>(drawTool)
{
  m_vertexShader =
    GetDrawTool().GetGPU().BuildSpirVShader("mesh_vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
  m_fragmentShader =
    GetDrawTool().GetGPU().BuildSpirVShader("mesh_frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
  CreatePipeline(format);
}

MeshRenderer::~MeshRenderer()
{
  auto * device = GetDrawTool().GetGPU().GetDevice();
  SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
  SDL_ReleaseGPUShader(device, m_fragmentShader);
  SDL_ReleaseGPUShader(device, m_vertexShader);
}

void MeshRenderer::PushObjectToDraw(IStaticMeshResource * mesh)
{
  m_drawCommands.push_back(mesh);
}

void MeshRenderer::RenderCache(SDL_GPURenderPass * renderPass)
{
  SDL_BindGPUGraphicsPipeline(renderPass, m_pipeline);
  for (auto && [path, drawData] : m_gpuCache)
  {
    // bind the vertex buffer
    SDL_GPUBufferBinding vertexBufferBinding;
    vertexBufferBinding.buffer = drawData.vertices; // index 0 is slot 0 in this example
    vertexBufferBinding.offset = 0;                 // start from the first byte
    SDL_GPUBufferBinding indexBufferBinding;
    indexBufferBinding.buffer = drawData.indices; // index 0 is slot 0 in this example
    indexBufferBinding.offset = 0;                // start from the first byte
    SDL_BindGPUVertexBuffers(renderPass, 0, &vertexBufferBinding,
                             1); // bind one buffer starting from slot 0
    SDL_BindGPUIndexBuffer(renderPass, &indexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

    for (auto && cmd : drawData.commands)
    {
      SDL_DrawGPUIndexedPrimitives(renderPass, static_cast<uint32_t>(cmd.indicesCount), 1,
                                   static_cast<uint32_t>(cmd.indicesOffset),
                                   static_cast<uint32_t>(cmd.verticesOffset), 0);
    }
  }
}

void MeshRenderer::UploadToGPU()
{
  for (auto && mesh : m_drawCommands)
  {
    auto * meshResource = dynamic_cast<IResource *>(mesh);
    auto [it, inserted] =
      m_gpuCache.insert({meshResource->GetPath(), StaticMeshGpuCache(*this, nullptr, 0)});
    size_t resourceDataHash = meshResource->Upload();
    if (inserted || resourceDataHash != it->second.dataHash)
    {
      it->second = StaticMeshGpuCache(*this, mesh, resourceDataHash);
    }
  }
}

void MeshRenderer::CreatePipeline(SDL_GPUTextureFormat format)
{
  SDL_GPUGraphicsPipelineCreateInfo pipelineInfo{};
  // bind shaders
  pipelineInfo.vertex_shader = m_vertexShader;
  pipelineInfo.fragment_shader = m_fragmentShader;
  // draw triangles
  pipelineInfo.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

  // describe the vertex buffers
  SDL_GPUVertexBufferDescription vertexBufferDesctiptions[1];
  vertexBufferDesctiptions[0].slot = 0;
  vertexBufferDesctiptions[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
  vertexBufferDesctiptions[0].instance_step_rate = 0;
  vertexBufferDesctiptions[0].pitch = sizeof(glm::vec3);
  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

  // describe the vertex attribute
  SDL_GPUVertexAttribute vertexAttributes[1];
  // a_position
  vertexAttributes[0].buffer_slot = 0; // fetch data from the buffer at slot 0
  vertexAttributes[0].location = 0;    // layout (location = 0) in shader
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3; //vec3
  vertexAttributes[0].offset = 0; // start from the first byte from current buffer position
  pipelineInfo.vertex_input_state.num_vertex_attributes = 1;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

  // describe the color target
  SDL_GPUColorTargetDescription colorTargetDescriptions[1];
  colorTargetDescriptions[0] = {};
  colorTargetDescriptions[0].format = format;
  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

  m_pipeline = SDL_CreateGPUGraphicsPipeline(GetDrawTool().GetGPU().GetDevice(), &pipelineInfo);
}


MeshRenderer::StaticMeshGpuCache::StaticMeshGpuCache(MeshRenderer & renderer,
                                                     IStaticMeshResource * mesh, size_t dataHash)
  : OwnedBy<MeshRenderer>(renderer)
  , dataHash(dataHash)
{
  if (!mesh)
    return;
  auto * device = GetRenderer().GetDrawTool().GetGPU().GetDevice();

  commands = mesh->GetPartsDescription();
  // create the vertex buffer
  {
    auto && meshVertices = mesh->GetVertices();
    const size_t verticesSize = meshVertices.size() * sizeof(glm::vec3);
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = verticesSize;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertices = SDL_CreateGPUBuffer(device, &bufferInfo);
    GetRenderer().GetDrawTool().GetGPU().GetUploader().UploadBuffer(vertices, 0,
                                                                    meshVertices.data(),
                                                                    verticesSize);
  }

  // create the index buffer
  {
    auto && meshIndices = mesh->GetIndices();
    const size_t indicesSize = meshIndices.size() * sizeof(uint32_t);
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = indicesSize;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indices = SDL_CreateGPUBuffer(device, &bufferInfo);
    GetRenderer().GetDrawTool().GetGPU().GetUploader().UploadBuffer(indices, 0, meshIndices.data(),
                                                                    indicesSize);
  }
}

MeshRenderer::StaticMeshGpuCache::~StaticMeshGpuCache()
{
  auto * device = GetRenderer().GetDrawTool().GetGPU().GetDevice();
  SDL_ReleaseGPUBuffer(device, vertices);
  SDL_ReleaseGPUBuffer(device, indices);
}

MeshRenderer::StaticMeshGpuCache::StaticMeshGpuCache(StaticMeshGpuCache && rhs) noexcept
  : OwnedBy<MeshRenderer>(std::forward<StaticMeshGpuCache>(rhs))
{
  std::swap(rhs.dataHash, dataHash);
  std::swap(rhs.commands, commands);
  std::swap(rhs.indices, indices);
  std::swap(rhs.vertices, vertices);
}

MeshRenderer::StaticMeshGpuCache & MeshRenderer::StaticMeshGpuCache::operator=(
  StaticMeshGpuCache && rhs) noexcept
{
  if (this != &rhs)
  {
    std::swap(rhs.dataHash, dataHash);
    std::swap(rhs.commands, commands);
    std::swap(rhs.indices, indices);
    std::swap(rhs.vertices, vertices);
    OwnedBy<MeshRenderer>::operator=(std::move(rhs));
  }
  return *this;
}

} // namespace GameFramework
