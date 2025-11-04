#pragma once
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
  void SetBackground(float r, float g, float b);
  void Submit();

private:
  RHI::ISubpass * m_renderPass = nullptr;
  RHI::IBufferGPU * m_colorBuffer = nullptr;
  RHI::IBufferUniformDescriptor * m_colorDescriptor = nullptr;
};
} // namespace RenderPlugin
