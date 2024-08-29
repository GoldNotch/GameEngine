#pragma once
#include <algorithm>
#include <memory>
#include <vector>

#include <Context.hpp>

namespace Graphics
{
/*
* Scene is a container for drawable objects.
* Internaly it's just command buffer
* System can create some kinds of Scene, and user can fill it in multithreaded
* But drawcall is single-threaded
*/

struct IScene
{
  virtual ~IScene() = default;
  //virtual std::vector<RHI::CommandBufferHandle> GetCommands() const = 0;
};

struct Scene : public IScene
{
  explicit Scene(const RHI::IContext & ctx) {}

  void PushStaticMesh(const StaticMesh & mesh);

private:
};

struct System final
{
  explicit System(const GraphicsSystemConfig & config)
    : m_context(RHI::CreateContext(RHI::SurfaceConfig{config.hWnd, config.hInstance}))
  {
    m_meshPipeline =
      m_context->CreatePipeline(m_context->GetSwapchain().GetDefaultFramebuffer(), 0);
    m_meshPipeline->AttachShader(RHI::ShaderType::Vertex, "Shaders/triangle.vert");
    m_meshPipeline->AttachShader(RHI::ShaderType::Fragment, "Shaders/triangle.frag");
    m_meshPipeline->Invalidate();
  }

  void Invalidate() { m_context->GetSwapchain().Invalidate(); }

  void DrawMeshes(StaticMesh mesh)
  {
    // this happends before DrawMesh call
    //BufferGPU vbo;
    //BufferGPU ibo;
    //vbo.upload(mesh.vertices, mesh.vertices_count);
    //ibo.upload(mesh.indices, mesh.indices_count);

    //auto && fbo = m_context->CreateFramebuffer();


    //ThreadLocalCommandBuffer buf; //contains second command buffer
    //buf.usePipeline(pipeline);    // like glUseShader
    //buf.bindVBO(vbo);
    //buf.bindIBO(ibo);
    //buf.drawIndexed(mesh.indices_count);
  }

  /// @brief glFlush actually
  /// @param scenes
  /// @param count
  void RenderAll(const IScene * scenes, size_t count)
  {
    //auto && renderer = m_context->GetSwapchain().GetDefaultFramebuffer();
    //auto && frame = renderer.AcquireFrame();
    //std::vector<RHI::CommandBufferHandle> total_buffer;
    //for (size_t i = 0; i < count; ++i)
    //{
    //  std::vector<RHI::CommandBufferHandle> buffer = scenes->GetCommands();
    //  total_buffer.insert(std::end(total_buffer), std::begin(buffer), std::end(buffer));
    //}
    //frame.Submit(std::move(total_buffer));
    //renderer.SwapFrame();
  }

  Scene & AcquireRenderScene() & noexcept
  {
    //TODO: don't create new scene. Alloc them once in constructor (one per frame) and use here
    return *std::make_unique<Scene>(*m_context).release();
  }

private:
  std::unique_ptr<RHI::IContext> m_context;
  std::unique_ptr<RHI::IPipeline> m_meshPipeline;
};

} // namespace Graphics
