#pragma once
#include <GameFramework.hpp>
#include <OwnedBy.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{
class BackgroundRenderer : public RHI::OwnedBy<RHI::IContext>,
                           public RHI::OwnedBy<RHI::IFramebuffer>
{
public:
  explicit BackgroundRenderer(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer);
  ~BackgroundRenderer();
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IContext, GetContext);
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IFramebuffer, GetFramebuffer);

public:
  void SetBackground(const GameFramework::Color3f & color);
  void Submit();

private:
  RHI::ISubpass * m_renderPass = nullptr;
  RHI::IBufferGPU * m_colorBuffer = nullptr;
  RHI::IBufferUniformDescriptor * m_colorDescriptor = nullptr;
};
} // namespace RenderPlugin
