#pragma once

#include <GameFramework.hpp>
#include <Render3D/Renderer/CubeRenderer.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{

struct ViewProjection final
{
  GameFramework::Mat4f view;
  GameFramework::Mat4f projection;
};

struct Scene3D_GPU final : public RHI::OwnedBy<RHI::IContext>,
                           public RHI::OwnedBy<RHI::IFramebuffer>
{
  explicit Scene3D_GPU(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer);
  ~Scene3D_GPU();
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IContext, GetContext);
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IFramebuffer, GetFramebuffer);

  void TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes);
  void SetCamera(const GameFramework::Camera & camera);

public:
    RHI::IBufferGPU* GetViewProjectionBuffer();

public:
  void Invalidate();
  void Draw();
  bool ShouldBeInvalidated() const noexcept;

private:
  RHI::IBufferGPU * m_viewProjBuffer = nullptr;
  CubeRenderer m_cubesRenderer;
};

} // namespace RenderPlugin
