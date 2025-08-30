#pragma once
#include <future>
#include <queue>
#include <span>

#include <OwnedBy.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct ConnectionGPU;

struct Uploader : public OwnedBy<ConnectionGPU>
{
  explicit Uploader(ConnectionGPU & connection);
  virtual ~Uploader() override;
  MAKE_ALIAS_FOR_GET_OWNER(ConnectionGPU, GetGPU);

  void SubmitAndUpload();

  void UploadBuffer(SDL_GPUBuffer * dstBuffer, size_t offset, const void * srcData, size_t size);
  using FillFunc = std::function<void(void *, size_t)>;
  void UploadBuffer(SDL_GPUBuffer * dstBuffer, size_t offset, size_t size, FillFunc && func);
  //std::future<std::vector<uint8_t>> Download(SDL_GPUBuffer * srcBuffer, size_t offset, size_t size);

private:
  using Task = std::pair<SDL_GPUTransferBufferLocation, SDL_GPUBufferRegion>;
  std::vector<Task> m_uploadTasks;

private:
  void FreeTransferBuffers();
};
} // namespace GameFramework
