#include "MeshRenderer.hpp"

#include <Resources/StaticMesh.hpp>

#include "DrawTool.hpp"

namespace GameFramework
{

MeshRenderer::MeshRenderer(DrawTool_SDL & drawTool, SDL_GPUTextureFormat format)
  : OwnedBy<DrawTool_SDL>(drawTool)
{
  auto * device = drawTool.GetDevice();
  m_vertexShader = GetDrawTool().BuildSpirVShader("quad_vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
  m_fragmentShader = GetDrawTool().BuildSpirVShader("quad_frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
  CreatePipeline(format);
}

MeshRenderer::~MeshRenderer()
{
  auto * device = GetDrawTool().GetDevice();
  SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
  SDL_ReleaseGPUShader(device, m_fragmentShader);
  SDL_ReleaseGPUShader(device, m_vertexShader);
}

void MeshRenderer::PushObjectToDraw(IResource * mesh)
{
  auto meshPtr = dynamic_cast<StaticMeshResource *>(mesh);
  if (!meshPtr)
  {
    Log(LogMessageType::Error, "Resource is not StaticMesh");
    return;
  }
  m_drawCommands.push_back(meshPtr);
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

    // issue a draw call
    SDL_DrawGPUIndexedPrimitives(renderPass, drawData.num_indices, 1, 0, 0, 0);
  }
}

void MeshRenderer::UploadToGPU()
{
  for (auto && meshResource : m_drawCommands)
  {
    auto [it, inserted] =
      m_gpuCache.insert({meshResource->GetPath(), StaticMeshGpuCache(*this, nullptr)});
    bool resourceDataHash = meshResource->Upload();
    if (inserted || resourceDataHash != it->second.dataHash)
    {
      it->second = StaticMeshGpuCache(*this, meshResource);
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
  vertexBufferDesctiptions[0].pitch = sizeof(Vertex);
  pipelineInfo.vertex_input_state.num_vertex_buffers = 1;
  pipelineInfo.vertex_input_state.vertex_buffer_descriptions = vertexBufferDesctiptions;

  // describe the vertex attribute
  SDL_GPUVertexAttribute vertexAttributes[1];
  // a_position
  vertexAttributes[0].buffer_slot = 0; // fetch data from the buffer at slot 0
  vertexAttributes[0].location = 0;    // layout (location = 0) in shader
  vertexAttributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2; //vec2
  vertexAttributes[0].offset = 0; // start from the first byte from current buffer position
  pipelineInfo.vertex_input_state.num_vertex_attributes = 1;
  pipelineInfo.vertex_input_state.vertex_attributes = vertexAttributes;

  // describe the color target
  SDL_GPUColorTargetDescription colorTargetDescriptions[1];
  colorTargetDescriptions[0] = {};
  colorTargetDescriptions[0].format = format;
  pipelineInfo.target_info.num_color_targets = 1;
  pipelineInfo.target_info.color_target_descriptions = colorTargetDescriptions;

  m_pipeline = SDL_CreateGPUGraphicsPipeline(GetDrawTool().GetDevice(), &pipelineInfo);
}


MeshRenderer::StaticMeshGpuCache::StaticMeshGpuCache(MeshRenderer & renderer,
                                                     StaticMeshResource * mesh)
  : OwnedBy<MeshRenderer>(renderer)
{
  if (!mesh)
    return;
  auto * device = GetRenderer().GetDrawTool().GetDevice();
  // create the vertex buffer
  {
    const size_t verticesSize = mesh->GetVerticesCount() * sizeof(Vertex);
    auto copyVertices = [mesh](void * dst, size_t size) {
        
    };
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = verticesSize;
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    vertices = SDL_CreateGPUBuffer(device, &bufferInfo);
    GetRenderer().GetDrawTool().GetUploader().UploadBuffer(vertices, 0, verticesSize, copyVertices);
  }

  // create the index buffer
  {
    const size_t indicesSize = mesh->GetIndicesCount() * sizeof(uint32_t);
    auto copyIndices = [mesh](void * dst, size_t size) {

    };
    SDL_GPUBufferCreateInfo bufferInfo{};
    bufferInfo.size = mesh->GetIndicesCount() * sizeof(uint32_t);
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_INDEX;
    indices = SDL_CreateGPUBuffer(device, &bufferInfo);
    GetRenderer().GetDrawTool().GetUploader().UploadBuffer(indices, 0, indicesSize, copyIndices);
  }
}

MeshRenderer::StaticMeshGpuCache::~StaticMeshGpuCache()
{
  auto * device = GetRenderer().GetDrawTool().GetDevice();
  SDL_ReleaseGPUBuffer(device, vertices);
  SDL_ReleaseGPUBuffer(device, indices);
}

MeshRenderer::StaticMeshGpuCache::StaticMeshGpuCache(StaticMeshGpuCache && rhs) noexcept
  : OwnedBy<MeshRenderer>(std::forward<StaticMeshGpuCache>(rhs))
{
  std::swap(rhs.dataHash, dataHash);
  std::swap(rhs.num_indices, num_indices);
  std::swap(rhs.num_vertices, num_vertices);
  std::swap(rhs.indices, indices);
  std::swap(rhs.vertices, vertices);
}

MeshRenderer::StaticMeshGpuCache & MeshRenderer::StaticMeshGpuCache::operator=(
  StaticMeshGpuCache && rhs) noexcept
{
  if (this != &rhs)
  {
    std::swap(rhs.dataHash, dataHash);
    std::swap(rhs.num_indices, num_indices);
    std::swap(rhs.num_vertices, num_vertices);
    std::swap(rhs.indices, indices);
    std::swap(rhs.vertices, vertices);
    OwnedBy<MeshRenderer>::operator=(std::move(rhs));
  }
  return *this;
}

} // namespace GameFramework
