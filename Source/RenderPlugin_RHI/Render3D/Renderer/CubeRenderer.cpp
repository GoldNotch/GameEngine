#include "CubeRenderer.hpp"

#include <algorithm>
#include <ranges>

#include <Constants.hpp>
#include <GameFramework.hpp>
#include <Render3D/Scene3D_GPU.hpp>
#include <ShaderFile.hpp>

namespace RenderPlugin
{

CubeRenderer::CubeRenderer(Scene3D_GPU & scene)
  : OwnedBy<Scene3D_GPU>(scene)
  , m_renderPass(scene.GetDevice().GetFramebuffer().CreateSubpass())
{
  m_matricesCpuBuffer.reserve(128);
  // должно заполняться ScreenDevice
  auto && subpassConfig = m_renderPass->GetConfiguration();

  subpassConfig.EnableDepthTest(true);
  subpassConfig.SetMeshTopology(RHI::MeshTopology::Triangle);
  subpassConfig.AddInputBinding(0, sizeof(GameFramework::Mat4f),
                                RHI::InputBindingType::InstanceData);
  subpassConfig.AddInputAttribute(0, 0, 0, 4, RHI::InputAttributeElementType::FLOAT);
  subpassConfig.AddInputAttribute(0, 1, 1 * sizeof(GameFramework::Vec4f), 4,
                                  RHI::InputAttributeElementType::FLOAT);
  subpassConfig.AddInputAttribute(0, 2, 2 * sizeof(GameFramework::Vec4f), 4,
                                  RHI::InputAttributeElementType::FLOAT);
  subpassConfig.AddInputAttribute(0, 3, 3 * sizeof(GameFramework::Vec4f), 4,
                                  RHI::InputAttributeElementType::FLOAT);
  {
    auto && stream = GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "Cube_vert.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Vertex, file.GetSpirV());
  }
  m_vpDescriptor = subpassConfig.DeclareUniform({0, 0}, RHI::ShaderType::Vertex);
  m_vpDescriptor->AssignBuffer(*scene.GetViewProjectionBuffer());

  // должно вызываться материалом
  {
    auto && stream = GameFramework::GetFileManager().OpenRead(g_shadersDirectory / "Cube_frag.spv");
    ShaderFile file;
    stream->ReadValue<ShaderFile>(file);
    subpassConfig.AttachShader(RHI::ShaderType::Fragment, file.GetSpirV());
  }
}

CubeRenderer::~CubeRenderer()
{
  //TODO: remove subpass
  //TODO: remove buffer
}

void CubeRenderer::TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes)
{
  if (newHash != m_hash)
  {
    size_t oldCapacity = m_matricesCpuBuffer.capacity();
    m_matricesCpuBuffer.clear();
    for (auto && cube : cubes)
      m_matricesCpuBuffer.push_back(cube.GetTransform());

    size_t newCapacity = m_matricesCpuBuffer.capacity();

    if (newCapacity != oldCapacity || !m_matricesBuffer)
    {
      RHI::IBufferGPU * newVerticesBuffer =
        GetScene().GetDevice().GetContext().AllocBuffer(newCapacity * sizeof(GameFramework::Mat4f),
                                                        RHI::BufferGPUUsage::VertexBuffer, false);
      //TODO: Delete old verticesBuffer
      m_matricesBuffer = newVerticesBuffer;
    }
    m_matricesBuffer->UploadAsync(m_matricesCpuBuffer.data(),
                                  m_matricesCpuBuffer.size() * sizeof(GameFramework::Mat4f));
    m_hash = newHash;
  }
}

void CubeRenderer::Submit()
{
  if (m_renderPass && m_renderPass->ShouldBeInvalidated() && !m_matricesCpuBuffer.empty())
  {
    auto extent = GetScene().GetDevice().GetFramebuffer().GetExtent();
    m_renderPass->BeginPass();
    m_renderPass->SetScissor(0, 0, extent[0], extent[1]);
    m_renderPass->SetViewport(static_cast<float>(extent[0]), static_cast<float>(extent[1]));
    m_renderPass->BindVertexBuffer(0, *m_matricesBuffer);
    m_renderPass->DrawVertices(36, m_matricesCpuBuffer.size());
    m_renderPass->EndPass();
  }
}

} // namespace RenderPlugin
