#pragma once
#include <functional>
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

using InternalObjectHandle = void *;

#define BIT(x) (1 << (x))

enum class ShaderType
{
  Vertex = BIT(0),
  TessellationControl = BIT(1),
  TessellationEvaluation = BIT(2),
  Geometry = BIT(3),
  Fragment = BIT(4),
  Compute = BIT(5),
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

enum class BufferGPUUsage
{
  VertexBuffer,
  IndexBuffer,
  UniformBuffer,
  StorageBuffer,
  TransformFeedbackBuffer,
  IndirectBuffer
};

enum class InputBindingType
{
  VertexData,
  InstanceData
};

enum class InputAttributeElementType
{
  FLOAT,
  DOUBLE,
  UINT,
  SINT
};

enum class IndexType
{
  UINT8,
  UINT16,
  UINT32
};

struct ICommandBuffer;
struct IBufferGPU;

struct IUniform
{
  virtual ~IUniform() = default;
  virtual void Upload(const void * data, size_t size, size_t offset = 0) = 0;
};

struct IPipeline
{
  virtual ~IPipeline() = default;
  // General static settings
  /// @brief attach shader to pipeline
  virtual void AttachShader(ShaderType type, const wchar_t * path) = 0;
  virtual void AddInputBinding(uint32_t slot, uint32_t stride, InputBindingType type) = 0;
  virtual void AddInputAttribute(uint32_t binding, uint32_t location, uint32_t offset,
                                 uint32_t elemsCount, InputAttributeElementType elemsType) = 0;

  virtual IBufferGPU * DeclareUniform(const char * name, uint32_t binding, ShaderType shaderStage,
                                      uint32_t size) = 0;

  /// @brief Rebuild object after settings were changed
  virtual void Invalidate() = 0;
  /// @brief Get subpass index
  virtual uint32_t GetSubpass() const = 0;
};


struct IFramebuffer
{
  virtual ~IFramebuffer() = default;
  /// @brief Swapchain calls that on resize
  virtual void SetExtent(uint32_t width, uint32_t height) = 0;

  /// @brief Rebuild object after settings were changed
  virtual void Invalidate() = 0;
  virtual InternalObjectHandle GetRenderPass() const = 0;
  virtual InternalObjectHandle GetHandle() const = 0;
};

/// @brief frames owner. API for rendering frames
struct ISwapchain
{
  virtual ~ISwapchain() = default;
  /// @brief Rebuild object after settings were changed
  virtual void Invalidate() = 0;
  /// @brief begin frame rendering. Returns buffer for drawing commands
  virtual ICommandBuffer * BeginFrame() = 0;
  /// @brief End frame rendering. Uploads commands on GPU
  virtual void EndFrame() = 0;
  /// @brief Get current extent (screen size)
  virtual std::pair<uint32_t, uint32_t> GetExtent() const = 0;
  /// @brief Get Default framebuffer
  virtual const IFramebuffer & GetDefaultFramebuffer() const & noexcept = 0;
  /// @brief Create thread local command buffer
  virtual std::unique_ptr<ICommandBuffer> CreateCommandBuffer() const = 0;
};

/// @brief buffer for GPU commands
struct ICommandBuffer
{
  virtual ~ICommandBuffer() = default;

  /// @brief draw vertices command
  virtual void DrawVertices(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex = 0,
                            uint32_t firstInstance = 0) const = 0;
  virtual void DrawIndexedVertices(uint32_t indexCount, uint32_t instanceCount,
                                   uint32_t firstIndex = 0, int32_t vertexOffset = 0,
                                   uint32_t firstInstance = 0) const = 0;
  /// @brief Set viewport command
  virtual void SetViewport(float width, float height) = 0;
  /// @brief Set scissor command
  virtual void SetScissor(int32_t x, int32_t y, uint32_t width, uint32_t height) = 0;
  virtual void BindVertexBuffer(uint32_t binding, const IBufferGPU & buffer,
                                uint32_t offset = 0) = 0;
  virtual void BindIndexBuffer(const IBufferGPU & buffer, IndexType type, uint32_t offset = 0) = 0;

  /// @brief Clears buffer
  virtual void Reset() const = 0;
  /// @brief Enable writing mode (only for thread local buffers). Also binds framebuffer and pipeline
  virtual void BeginWriting(const IFramebuffer & framebuffer, const IPipeline & pipeline) = 0;
  /// @brief disables writing mode
  virtual void EndWriting() = 0;
  /// @brief add commands
  virtual void AddCommands(const ICommandBuffer & buffer) const = 0;
  /// @brief get type of buffer
  virtual CommandBufferType GetType() const noexcept = 0;
};

struct IBufferGPU
{
  using UnmapFunc = std::function<void(void *)>;
  using ScopedPointer = std::unique_ptr<void, UnmapFunc>;

  virtual ~IBufferGPU() = default;
  virtual ScopedPointer Map() = 0;
  virtual void Flush() const noexcept = 0;
  virtual bool IsMapped() const noexcept = 0;
  virtual InternalObjectHandle GetHandle() const noexcept = 0;
  virtual size_t Size() const noexcept = 0;
};


struct IContext
{
  virtual ~IContext() = default;
  virtual ISwapchain & GetSwapchain() & noexcept = 0;
  virtual const ISwapchain & GetSwapchain() const & noexcept = 0;
  virtual void WaitForIdle() const = 0;

  /// @brief create offscreen framebuffer
  virtual std::unique_ptr<IFramebuffer> CreateFramebuffer() const = 0;
  /// @brief create new pipeline
  virtual std::unique_ptr<IPipeline> CreatePipeline(const IFramebuffer & framebuffer,
                                                    uint32_t subpassIndex) const = 0;

  virtual std::unique_ptr<IBufferGPU> AllocBuffer(size_t size, BufferGPUUsage usage,
                                                  bool mapped = false) const = 0;
};

std::unique_ptr<IContext> CreateContext(const SurfaceConfig & config);

} // namespace RHI
