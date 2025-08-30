#include "QuadRenderer.hpp"

#include <SDL/Rendering/ConnectionGPU.hpp>
#include <SDL/Rendering/RenderTarget.hpp>

#include "DrawTool.hpp"

namespace
{
constexpr Uint32 c_bufferSize = 1024;
}

namespace GameFramework
{

QuadRenderer::QuadRenderer(DrawTool_SDL & drawTool)
  : OwnedBy<DrawTool_SDL>(drawTool)
{
  // create the vertex buffer
  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = c_bufferSize;
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  m_gpuData = SDL_CreateGPUBuffer(GetDrawTool().GetGPU().GetDevice(), &bufferInfo);

  m_vertexShader =
    GetDrawTool().GetGPU().BuildSpirVShader("quad_vert.spv", SDL_GPU_SHADERSTAGE_VERTEX);
  m_fragmentShader =
    GetDrawTool().GetGPU().BuildSpirVShader("quad_frag.spv", SDL_GPU_SHADERSTAGE_FRAGMENT);
}

QuadRenderer::~QuadRenderer()
{
  auto * device = GetDrawTool().GetGPU().GetDevice();
  SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
  SDL_ReleaseGPUShader(device, m_fragmentShader);
  SDL_ReleaseGPUShader(device, m_vertexShader);
  SDL_ReleaseGPUBuffer(device, m_gpuData);
}

void QuadRenderer::PushObjectToDraw(const Rect & rect)
{
  m_rectsToDraw.push_back(rect);
}

void QuadRenderer::RenderCache(SDL_GPUCommandBuffer * cmdBuf)
{
  InvalidatePipeline();
  auto && renderTarget = GetDrawTool().GetRenderTarget();
  auto renderPass = renderTarget.CreateRenderPass(cmdBuf);
  SDL_BindGPUGraphicsPipeline(renderPass.get(), m_pipeline);

  // bind the vertex buffer
  SDL_GPUBufferBinding bufferBindings[1];
  bufferBindings[0].buffer = m_gpuData; // index 0 is slot 0 in this example
  bufferBindings[0].offset = 0;         // start from the first byte
  SDL_BindGPUVertexBuffers(renderPass.get(), 0, bufferBindings,
                           1); // bind one buffer starting from slot 0

  // issue a draw call
  SDL_DrawGPUPrimitives(renderPass.get(), m_rectsToDraw.size() * 2 * 3, 1, 0, 0);
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

  GetDrawTool().GetGPU().GetUploader().UploadBuffer(m_gpuData, 0, vertices.data(),
                                                    vertices.size() * sizeof(glm::vec2));
}

void QuadRenderer::InvalidatePipeline()
{
  auto && renderTarget = GetDrawTool().GetRenderTarget();
  if (!m_pipeline || renderTarget.ShouldRebuildPipelines())
  {
    if (m_pipeline)
    {
      auto * device = GetDrawTool().GetGPU().GetDevice();
      SDL_ReleaseGPUGraphicsPipeline(device, m_pipeline);
    }
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

    // describe the color targets
    pipelineInfo.target_info = renderTarget.GetTargetsInfo();

    m_pipeline = SDL_CreateGPUGraphicsPipeline(GetDrawTool().GetGPU().GetDevice(), &pipelineInfo);
  }
}

} // namespace GameFramework
