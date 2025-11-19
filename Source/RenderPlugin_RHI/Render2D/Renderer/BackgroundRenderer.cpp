#include "BackgroundRenderer.hpp"

#include <Constants.hpp>
#include <GameFramework.hpp>
#include <Render2D/Scene2D_GPU.hpp>
#include <ShaderFile.hpp>

namespace RenderPlugin
{
BackgroundRenderer::BackgroundRenderer(Scene2D_GPU & scene)
  : OwnedBy<Scene2D_GPU>(scene)
  , m_renderPass(scene.GetDevice().GetFramebuffer().CreateSubpass())
  , m_colorBuffer(scene.GetDevice().GetContext().AllocBuffer(3 * sizeof(float),
                                                             RHI::BufferGPUUsage::UniformBuffer,
                                                             true))
{
  auto && subpassConfig = m_renderPass->GetConfiguration();
  subpassConfig.BindAttachment(0, RHI::ShaderAttachmentSlot::Color);
  subpassConfig.BindAttachment(1, RHI::ShaderAttachmentSlot::DepthStencil);
  subpassConfig.BindResolver(2, 0);
  subpassConfig.EnableDepthTest(true);
  subpassConfig.SetMeshTopology(RHI::MeshTopology::TriangleFan);
  m_colorDescriptor = subpassConfig.DeclareUniform({0, 0}, RHI::ShaderType::Fragment);
  m_colorDescriptor->AssignBuffer(*m_colorBuffer);
  {
    auto && stream =
      GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "background_vert.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Vertex, file.GetSpirV());
  }
  {
    auto && stream =
      GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "background_frag.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Fragment, file.GetSpirV());
  }
}

BackgroundRenderer::~BackgroundRenderer()
{
  //TODO: remove subpass
  //TODO: remove buffer
}

void BackgroundRenderer::SetBackground(const GameFramework::Color3f & color)
{
  m_colorBuffer->UploadSync(color.data(), color.size() * sizeof(float));
}

void BackgroundRenderer::Submit()
{
  if (m_renderPass && m_renderPass->ShouldBeInvalidated())
  {
    auto extent = GetScene().GetDevice().GetFramebuffer().GetExtent();
    m_renderPass->BeginPass();
    m_renderPass->SetScissor(0, 0, extent[0], extent[1]);
    m_renderPass->SetViewport(static_cast<float>(extent[0]), static_cast<float>(extent[1]));
    m_renderPass->DrawVertices(4, 1);
    m_renderPass->EndPass();
  }
}

} // namespace RenderPlugin
