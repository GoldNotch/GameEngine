#pragma once
#include <GameFramework.hpp>
#include <OwnedBy.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{
class Rect2DRenderer : public RHI::OwnedBy<RHI::IContext>,
                       public RHI::OwnedBy<RHI::IFramebuffer>
{
public:
  explicit Rect2DRenderer(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer);
  ~Rect2DRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IContext, GetContext);
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IFramebuffer, GetFramebuffer);

public:
  void TrySetRects(size_t newHash, std::span<const GameFramework::Rect2d> rects);
  void Submit();

private:
  size_t m_hash = 0;
  std::vector<std::pair<float, float>> m_verticesCpuBuffer;
  RHI::ISubpass * m_renderPass = nullptr;
  RHI::IBufferGPU * m_verticesBuffer = nullptr;
};
} // namespace RenderPlugin
