#pragma once

#include <OwnedBy.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{
struct InternalDevice : public RHI::OwnedBy<RHI::IContext>
{
  explicit InternalDevice(RHI::IContext & ctx)
    : OwnedBy<RHI::IContext>(ctx)
  {
  }
  virtual ~InternalDevice() = default;
  MAKE_ALIAS_FOR_GET_OWNER(RHI::IContext, GetContext);

public:
  virtual void ConfigurePipeline(RHI::ISubpassConfiguration & config) const = 0;
  virtual RHI::IFramebuffer & GetFramebuffer() & noexcept = 0;
};
} // namespace RenderPlugin
