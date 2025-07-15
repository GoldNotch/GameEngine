#include "QuadRenderer.hpp"

namespace
{
constexpr Uint32 c_bufferSize = 1024;
}

namespace GameFramework
{

QuadRenderer::QuadRenderer(SDL_GPUDevice * gpu)
  : m_gpu(gpu)
{
  // create the vertex buffer
  SDL_GPUBufferCreateInfo bufferInfo{};
  bufferInfo.size = c_bufferSize;
  bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
  m_gpuData = SDL_CreateGPUBuffer(m_gpu, &bufferInfo);
}

QuadRenderer::~QuadRenderer()
{
  SDL_ReleaseGPUBuffer(m_gpu, m_gpuData);
}

void QuadRenderer::PushObjectToDraw(const Rect & rect)
{
  m_rectsToDraw.push_back(rect);
}

void QuadRenderer::RenderCache()
{

  m_rectsToDraw.clear();
}

void QuadRenderer::UploadToGPU(SDL_GPUCopyPass * copyPass)
{
  const Uint32 c_bufferSize = sizeof(Vertex) * 2 * m_rectsToDraw.size();

  // create a transfer buffer to upload to the vertex buffer
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = c_bufferSize;
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  SDL_GPUTransferBuffer * transferBuffer = SDL_CreateGPUTransferBuffer(m_gpu, &transferInfo);

  // map the transfer buffer to a pointer
  Vertex * data =
    reinterpret_cast<Vertex *>(SDL_MapGPUTransferBuffer(m_gpu, transferBuffer, false));
  for (const auto& rect : m_rectsToDraw)
  {
    *(data++) = {rect.left, rect.top};
    *(data++) = {rect.left, rect.bottom};
    *(data++) = {rect.right, rect.top};

    *(data++) = {rect.right, rect.top};
    *(data++) = {rect.left, rect.bottom};
    *(data++) = {rect.right, rect.bottom};
  }
  // unmap the pointer when you are done updating the transfer buffer
  SDL_UnmapGPUTransferBuffer(m_gpu, transferBuffer);

  // where is the data
  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  location.offset = 0; // start from the beginning
  // where to upload the data
  SDL_GPUBufferRegion region{};
  region.buffer = m_gpuData;
  region.size = c_bufferSize; // size of the data in bytes
  region.offset = 0;              // begin writing from the first vertex
  // upload the data
  SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  SDL_ReleaseGPUTransferBuffer(m_gpu, transferBuffer);
}

} // namespace GameFramework
