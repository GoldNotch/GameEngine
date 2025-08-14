#include "QuadRenderer.hpp"

#include "DrawTool.hpp"

namespace
{
constexpr Uint32 c_bufferSize = 1024;
}

namespace GameFramework
{

QuadRenderer::QuadRenderer(DrawTool_SDL & drawTool, SDL_GPUTextureFormat format)
  : OwnedBy<DrawTool_SDL>(drawTool)
{
  auto * device = drawTool.GetDevice();
  // create the vertex buffer
  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = c_bufferSize;
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  m_gpuData = SDL_CreateGPUBuffer(drawTool.GetDevice(), &bufferInfo);

  m_vertexShader = GetDrawTool().BuildSpirVShader("quad_vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
  m_fragmentShader = GetDrawTool().BuildSpirVShader("quad_frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
  CreatePipeline(format);
}

QuadRenderer::~QuadRenderer()
{
  auto * device = GetDrawTool().GetDevice();
  SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
  SDL_ReleaseGPUShader(device, m_fragmentShader);
  SDL_ReleaseGPUShader(device, m_vertexShader);
  SDL_ReleaseGPUBuffer(device, m_gpuData);
}

void QuadRenderer::PushObjectToDraw(const Rect & rect)
{
  m_rectsToDraw.push_back(rect);
}

void QuadRenderer::RenderCache(SDL_GPURenderPass * renderPass)
{
  SDL_BindGPUGraphicsPipeline(renderPass, m_pipeline);

  // bind the vertex buffer
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = m_gpuData; // index 0 is slot 0 in this example
  bufferBindings[0].offset = 0;         // start from the first byte
  SDL_BindGPUVertexBuffers(renderPass, 0, bufferBindings,
                           1); // bind one buffer starting from slot 0

  // issue a draw call
  SDL_DrawGPUPrimitives(renderPass, m_rectsToDraw.size() * 2 * 3, 1, 0, 0);

  m_rectsToDraw.clear();
}

void QuadRenderer::UploadToGPU()
{
  const Uint32 bufferSize = sizeof(glm::vec2) * 3 * 2 * m_rectsToDraw.size();
  std::vector<glm::vec2> vertices;
  vertices.reserve(3 * 2 * m_rectsToDraw.size());
  for (auto && rect : m_rectsToDraw)
  {
    vertices.push_back({rect.left, rect.top});
    vertices.push_back({rect.left, rect.bottom});
    vertices.push_back({rect.right, rect.top});

    vertices.push_back({rect.right, rect.top});
    vertices.push_back({rect.left, rect.bottom});
    vertices.push_back({rect.right, rect.bottom});
  }

  GetDrawTool().GetUploader().UploadBuffer(m_gpuData, 0, vertices.data(),
                                           vertices.size() * sizeof(glm::vec2));
}

void QuadRenderer::CreatePipeline(SDL_GPUTextureFormat format)
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
  vertexBufferDesctiptions[0].pitch = sizeof(glm::vec2);
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

} // namespace GameFramework
