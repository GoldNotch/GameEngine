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

using RenderPassHandle = void *;

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

enum class ShaderImageSlot
{
  Color,
  DepthStencil,
  Input,
  TOTAL
};

enum class CommandBufferType
{
  Executable, ///< executable in GPU
  ThreadLocal ///< filled in separate thread
};

struct ICommandBuffer;

struct IPipeline
{
  virtual ~IPipeline() = default;
  virtual void AttachShader(ShaderType type, const wchar_t * path) = 0;
  virtual void Invalidate() = 0;

  virtual uint32_t GetSubpass() const = 0;
};

struct IFramebuffer
{
  virtual ~IFramebuffer() = default;
  virtual void SetExtent(uint32_t width, uint32_t height) = 0;
  virtual void Invalidate() = 0;
  virtual RenderPassHandle GetRenderPass() const = 0;
  virtual RenderPassHandle GetHandle() const = 0;
};

/// @brief frames owner. It's a queue (or swapchain)
struct ISwapchain
{
  virtual ~ISwapchain() = default;
  virtual void Invalidate() = 0;
  virtual ICommandBuffer * BeginFrame() = 0; ///< Begin Rendering
  virtual void EndFrame() = 0;  ///< End Rendering
  virtual std::pair<uint32_t, uint32_t> GetExtent() const = 0;


  virtual const IFramebuffer & GetDefaultFramebuffer() const & noexcept = 0;
  virtual std::unique_ptr<ICommandBuffer> CreateCommandBuffer() const = 0;
};

struct ICommandBuffer
{
  virtual ~ICommandBuffer() = default;

  virtual void DrawVertices(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
                            uint32_t firstInstance = 0) const = 0;
  virtual void SetViewport(float width, float height) = 0;
  virtual void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;

  virtual void Reset() const = 0;
  
  virtual void BeginWriting(const IFramebuffer& framebuffer, const IPipeline & pipeline) = 0;
  virtual void EndWriting() = 0;
  virtual void AddCommands(const ICommandBuffer & buffer) const = 0;
  virtual CommandBufferType GetType() const noexcept = 0;
};

struct IContext
{
  virtual ~IContext() = default;
  virtual ISwapchain & GetSwapchain() & noexcept = 0;
  virtual const ISwapchain & GetSwapchain() const & noexcept = 0;
  virtual void WaitForIdle() const = 0;

  virtual std::unique_ptr<IFramebuffer> CreateFramebuffer() const = 0;
  virtual std::unique_ptr<IPipeline> CreatePipeline(const IFramebuffer & framebuffer,
                                                    uint32_t subpassIndex) const = 0;
};

std::unique_ptr<IContext> CreateContext(const SurfaceConfig & config);

} // namespace RHI
