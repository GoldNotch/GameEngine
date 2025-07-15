#include "QuadRenderer.hpp"

namespace
{
constexpr Uint32 c_bufferSize = 1024;
}

namespace GameFramework
{

QuadRenderer::QuadRenderer(SDL_GPUDevice * gpu)
  : m_gpu(gpu)
{
  // create the vertex buffer
  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = c_bufferSize;
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  m_gpuData = SDL_CreateGPUBuffer(m_gpu, &bufferInfo);

  // load the vertex shader code
  size_t vertexCodeSize;
  void * vertexCode = SDL_LoadFile("quad.vert.spv", &vertexCodeSize);
  // create the vertex shader
  SDL_GPUShaderCreateInfo vertexInfo{};
  vertexInfo.code = (Uint8 *)vertexCode; //convert to an array of bytes
  vertexInfo.code_size = vertexCodeSize;
  vertexInfo.entrypoint = "main";
  vertexInfo.format = SDL_GPU_SHADERFORMAT_SPIRV; // loading .spv shaders
  vertexInfo.stage = SDL_GPU_SHADERSTAGE_VERTEX;  // vertex shader
  vertexInfo.num_samplers = 0;
  vertexInfo.num_storage_buffers = 0;
  vertexInfo.num_storage_textures = 0;
  vertexInfo.num_uniform_buffers = 0;
  m_vertexShader = SDL_CreateGPUShader(m_gpu, &vertexInfo);
  // free the file
  SDL_free(vertexCode);

  // create the fragment shader
  size_t fragmentCodeSize;
  void * fragmentCode = SDL_LoadFile("quad.frag.spv", &fragmentCodeSize);
  // create the fragment shader
  SDL_GPUShaderCreateInfo fragmentInfo{};
  fragmentInfo.code = (Uint8 *)fragmentCode;
  fragmentInfo.code_size = fragmentCodeSize;
  fragmentInfo.entrypoint = "main";
  fragmentInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
  fragmentInfo.stage = SDL_GPU_SHADERSTAGE_FRAGMENT; // fragment shader
  fragmentInfo.num_samplers = 0;
  fragmentInfo.num_storage_buffers = 0;
  fragmentInfo.num_storage_textures = 0;
  fragmentInfo.num_uniform_buffers = 0;
  m_fragmentShader = SDL_CreateGPUShader(m_gpu, &fragmentInfo);
  // free the file
  SDL_free(fragmentCode);
}

QuadRenderer::~QuadRenderer()
{
  SDL_ReleaseGPUShader(m_gpu, m_fragmentShader);
  SDL_ReleaseGPUShader(m_gpu, m_vertexShader);
  SDL_ReleaseGPUBuffer(m_gpu, m_gpuData);
}

void QuadRenderer::PushObjectToDraw(const Rect & rect)
{
  m_rectsToDraw.push_back(rect);
}

void QuadRenderer::RenderCache(SDL_GPURenderPass * renderPass, SDL_GPUTextureFormat format)
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

  SDL_GPUGraphicsPipeline * graphicsPipeline = SDL_CreateGPUGraphicsPipeline(m_gpu, &pipelineInfo);
  SDL_BindGPUGraphicsPipeline(renderPass, graphicsPipeline);

  // bind the vertex buffer
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = m_gpuData; // index 0 is slot 0 in this example
  bufferBindings[0].offset = 0;         // start from the first byte
  SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings, 1); // bind one buffer starting from slot 0

  // issue a draw call
  SDL_DrawGPUPrimitives(renderPass, m_rectsToDraw.size() * 2 * 3, 1, 0, 0);

  SDL_ReleaseGPUGraphicsPipeline(m_gpu, graphicsPipeline);

  m_rectsToDraw.clear();
}

void QuadRenderer::UploadToGPU(SDL_GPUCopyPass * copyPass)
{
  const Uint32 bufferSize = sizeof(Vertex) * 3 * 2 * m_rectsToDraw.size();

  // create a transfer buffer to upload to the vertex buffer
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = bufferSize;
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  SDL_GPUTransferBuffer * transferBuffer = SDL_CreateGPUTransferBuffer(m_gpu, &transferInfo);

  // map the transfer buffer to a pointer
  Vertex * data =
    reinterpret_cast<Vertex *>(SDL_MapGPUTransferBuffer(m_gpu, transferBuffer, false));
  for (const auto& rect : m_rectsToDraw)
  {
    *(data++) = {rect.left, rect.top};
    *(data++) = {rect.left, rect.bottom};
    *(data++) = {rect.right, rect.top};

    *(data++) = {rect.right, rect.top};
    *(data++) = {rect.left, rect.bottom};
    *(data++) = {rect.right, rect.bottom};
  }
  // unmap the pointer when you are done updating the transfer buffer
  SDL_UnmapGPUTransferBuffer(m_gpu, transferBuffer);

  // where is the data
  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  location.offset = 0; // start from the beginning
  // where to upload the data
  SDL_GPUBufferRegion region{};
  region.buffer = m_gpuData;
  region.size = bufferSize; // size of the data in bytes
  region.offset = 0;        // begin writing from the first vertex
  // upload the data
  SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  SDL_ReleaseGPUTransferBuffer(m_gpu, transferBuffer);
}

} // namespace GameFramework
