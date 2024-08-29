#pragma once
#include <memory>
#include <vector>

namespace RHI
{

using ExternalHandle = void *;

struct SurfaceConfig
{
  ExternalHandle hWnd;
  ExternalHandle hInstance;
};

using CommandBufferHandle = void *;

/// @brief One swapchain's frame
struct IFrameData
{
  //void PushCommand();
  // contains CommandBuffer and you can write it from several threads
  virtual ~IFrameData() = default;
  virtual void Submit(std::vector<CommandBufferHandle> && buffers)
    const = 0; // send command buffer to GPU and render it
  virtual void WaitForRenderingComplete() const = 0;
};

enum class ShaderType
{
  Vertex,
  TessellationControl,
  TessellationEvaluation,
  Geometry,
  Fragment,
  Compute,
};

enum class MeshTopology
{
  Point,
  Line,
  LineStrip,
  Triangle,
  TriangleFan,
  TriangleStrip
};

enum class PolygonMode
{
  Fill,
  Line,
  Point
};

enum class FrontFace
{
  CW, //clockwise
  CCW // counter clockwise
};

enum class CullingMode
{
  None,
  FrontFace,
  BackFace,
  FrontAndBack
};

enum class BlendOperation
{
  Add,              // src.color + dst.color
  Subtract,         // src.color - dst.color
  ReversedSubtract, //dst.color - src.color
  Min,              // min(src.color, dst.color)
  Max               // max(src.color, dst.color)
};

enum class BlendFactor
{
  Zero,
  One,
  SrcColor,
  OneMinusSrcColor,
  DstColor,
  OneMinusDstColor,
  SrcAlpha,
  OneMinusSrcAlpha,
  DstAlpha,
  OneMinusDstAlpha,
  ConstantColor,
  OneMinusConstantColor,
  ConstantAlpha,
  OneMinusConstantAlpha,
  SrcAlphaSaturate,
  Src1Color,
  OneMinusSrc1Color,
  Src1Alpha,
  OneMinusSrc1Alpha,
};

struct IPipeline
{
  virtual ~IPipeline() = default;
  virtual void AttachShader(ShaderType type, const char * path) = 0;
  virtual void Invalidate() = 0;
};

struct IFramebuffer
{
  virtual ~IFramebuffer() = default;
  virtual void SetExtent(uint32_t width, uint32_t height) = 0;
  virtual void Invalidate() = 0;
  virtual void BeginRendering(CommandBufferHandle cmds) const = 0;
  virtual void EndRendering(CommandBufferHandle cmds) const = 0;
};

/// @brief frames owner. It's a queue (or swapchain)
struct ISwapchain
{
  virtual ~ISwapchain() = default;
  virtual void Invalidate() = 0;
  //virtual IFrameData & AcquireFrame() & = 0;
  virtual void SwapBuffers() = 0;
  virtual const IFramebuffer & GetDefaultFramebuffer() const & noexcept = 0;
};

struct IContext
{
  virtual ~IContext() = default;
  virtual ISwapchain & GetSwapchain() & noexcept = 0;
  virtual const ISwapchain & GetSwapchain() const & noexcept = 0;
  virtual std::unique_ptr<IFramebuffer> CreateFramebuffer() const = 0;
  virtual std::unique_ptr<IPipeline> CreatePipeline(const IFramebuffer & framebuffer,
                                                    uint32_t subpassIndex) const = 0;
};

std::unique_ptr<IContext> CreateContext(const SurfaceConfig & config);

} // namespace RHI
