#include "Rect2DRenderer.hpp"

#include <Constants.hpp>
#include <GameFramework.hpp>
#include <Render2D/Scene2D_GPU.hpp>
#include <ShaderFile.hpp>

namespace RenderPlugin
{
Rect2DRenderer::Rect2DRenderer(Scene2D_GPU & scene)
  : OwnedBy<Scene2D_GPU>(scene)
  , m_renderPass(scene.GetDevice().GetFramebuffer().CreateSubpass())
{
  m_verticesCpuBuffer.reserve(128);
  auto && subpassConfig = m_renderPass->GetConfiguration();
  subpassConfig.BindAttachment(0, RHI::ShaderAttachmentSlot::Color);
  subpassConfig.BindAttachment(1, RHI::ShaderAttachmentSlot::DepthStencil);
  subpassConfig.BindResolver(2, 0);
  subpassConfig.EnableDepthTest(true);
  subpassConfig.SetMeshTopology(RHI::MeshTopology::Triangle);
  subpassConfig.AddInputBinding(0, 2 * sizeof(float), RHI::InputBindingType::VertexData);
  subpassConfig.AddInputAttribute(0, 0, 0, 2, RHI::InputAttributeElementType::FLOAT);
  {
    auto && stream =
      GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "rect2d_vert.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Vertex, file.GetSpirV());
  }
  {
    auto && stream =
      GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "rect2d_frag.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Fragment, file.GetSpirV());
  }
}

Rect2DRenderer::~Rect2DRenderer()
{
  //TODO: remove subpass
  //TODO: remove buffer
}


void Rect2DRenderer::TrySetRects(size_t newHash, std::span<const GameFramework::Rect2d> rects)
{
  if (newHash != m_hash)
  {
    size_t oldCapacity = m_verticesCpuBuffer.capacity();
    m_verticesCpuBuffer.clear();
    for (auto && rect : rects)
    {
      const float l = rect.X();
      const float t = rect.Y();
      const float r = rect.X() + rect.Width();
      const float b = rect.Y() + rect.Height();
      m_verticesCpuBuffer.emplace_back(l, t);
      m_verticesCpuBuffer.emplace_back(r, t);
      m_verticesCpuBuffer.emplace_back(l, b);
      m_verticesCpuBuffer.emplace_back(l, b);
      m_verticesCpuBuffer.emplace_back(r, t);
      m_verticesCpuBuffer.emplace_back(r, b);
    }

    size_t newCapacity = m_verticesCpuBuffer.capacity();

    if (newCapacity != oldCapacity || !m_verticesBuffer)
    {
      RHI::IBufferGPU * newVerticesBuffer =
        GetScene().GetDevice().GetContext().AllocBuffer(newCapacity * 6 * 2 * sizeof(float),
                                                        RHI::BufferGPUUsage::VertexBuffer, false);
      //TODO: Delete old verticesBuffer
      m_verticesBuffer = newVerticesBuffer;
    }
    m_verticesBuffer->UploadAsync(m_verticesCpuBuffer.data(),
                                  m_verticesCpuBuffer.size() * 6 * 2 * sizeof(float));
    m_hash = newHash;
  }
}

void Rect2DRenderer::Submit()
{
  if (m_renderPass && m_renderPass->ShouldBeInvalidated() && !m_verticesCpuBuffer.empty())
  {
    auto extent = GetScene().GetDevice().GetFramebuffer().GetExtent();
    m_renderPass->BeginPass();
    m_renderPass->SetScissor(0, 0, extent[0], extent[1]);
    m_renderPass->SetViewport(static_cast<float>(extent[0]), static_cast<float>(extent[1]));
    m_renderPass->BindVertexBuffer(0, *m_verticesBuffer);
    m_renderPass->DrawVertices(m_verticesCpuBuffer.size(), 1);
    m_renderPass->EndPass();
  }
}

} // namespace RenderPlugin
