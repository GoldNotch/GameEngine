#include "Uploader.hpp"

#include "ConnectionGPU.hpp"

namespace GameFramework
{
Uploader::Uploader(ConnectionGPU & connection)
  : OwnedBy<ConnectionGPU>(connection)
{
}

Uploader::~Uploader()
{
  FreeTransferBuffers();
}

void Uploader::SubmitAndUpload()
{
  auto * device = GetGPU().GetDevice();

  SDL_GPUCommandBuffer * commandBuffer = SDL_AcquireGPUCommandBuffer(device);
  SDL_GPUCopyPass * copyPass = SDL_BeginGPUCopyPass(commandBuffer);

  for (auto && [location, region] : m_uploadTasks)
    SDL_UploadToGPUBuffer(copyPass, &location, &region, true);

  // end the copy pass
  SDL_EndGPUCopyPass(copyPass);
  SDL_GPUFence * fence = SDL_SubmitGPUCommandBufferAndAcquireFence(commandBuffer);

  // wait while all commands are proceed
  SDL_WaitForGPUFences(device, true, &fence, 1);
  SDL_ReleaseGPUFence(device, fence);
  FreeTransferBuffers();
}

void Uploader::UploadBuffer(SDL_GPUBuffer * dstBuffer, size_t offset, const void * srcData,
                            size_t size)
{
  auto * device = GetGPU().GetDevice();

  // create a transfer buffer to upload to the buffer
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = static_cast<uint32_t>(size);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  SDL_GPUTransferBuffer * transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
  if (!transferBuffer)
    throw std::runtime_error("Failed to create a transfer buffer");
  //TODO: make a pool of TransferBuffers

  // map the transfer buffer to a pointer
  char * dstData =
    reinterpret_cast<char *>(SDL_MapGPUTransferBuffer(device, transferBuffer, false));
  SDL_memcpy(dstData, srcData, size);
  // unmap the pointer when you are done updating the transfer buffer
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  SDL_GPUBufferRegion dstRegion{};
  dstRegion.buffer = dstBuffer;
  dstRegion.offset = static_cast<uint32_t>(offset); // start from the beginning
  dstRegion.size = static_cast<uint32_t>(size);

  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  location.offset = 0;

  m_uploadTasks.push_back({location, dstRegion});
}

void Uploader::UploadBuffer(SDL_GPUBuffer * dstBuffer, size_t offset, size_t size, FillFunc && fill)
{
  auto * device = GetGPU().GetDevice();

  // create a transfer buffer to upload to the buffer
  SDL_GPUTransferBufferCreateInfo transferInfo{};
  transferInfo.size = static_cast<uint32_t>(size);
  transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
  SDL_GPUTransferBuffer * transferBuffer = SDL_CreateGPUTransferBuffer(device, &transferInfo);
  //TODO: make a pool of TransferBuffers

  // map the transfer buffer to a pointer
  void * dstData = SDL_MapGPUTransferBuffer(device, transferBuffer, false);
  fill(dstData, size);
  // unmap the pointer when you are done updating the transfer buffer
  SDL_UnmapGPUTransferBuffer(device, transferBuffer);

  SDL_GPUBufferRegion dstRegion{};
  dstRegion.buffer = dstBuffer;
  dstRegion.offset = static_cast<uint32_t>(offset); // start from the beginning
  dstRegion.size = static_cast<uint32_t>(size);

  SDL_GPUTransferBufferLocation location{};
  location.transfer_buffer = transferBuffer;
  location.offset = 0;

  m_uploadTasks.push_back({location, dstRegion});
}

void Uploader::FreeTransferBuffers()
{
  auto * device = GetGPU().GetDevice();
  //TODO: make a pool of TransferBuffers
  for (auto && [location, region] : m_uploadTasks)
  {
    SDL_ReleaseGPUTransferBuffer(device, location.transfer_buffer);
  }
  m_uploadTasks.clear();
}


} // namespace GameFramework
