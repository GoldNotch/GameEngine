#pragma once
#include <future>
#include <queue>

#include <OwnedBy.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct DrawTool_SDL;

struct Uploader : public OwnedBy<DrawTool_SDL>
{
  explicit Uploader(DrawTool_SDL & drawTool);
  virtual ~Uploader() override;
  MAKE_ALIAS_FOR_GET_OWNER(DrawTool_SDL, GetDrawTool);

  void SubmitAndUpload();

  void UploadBuffer(SDL_GPUBuffer * dstBuffer, size_t offset, const void * srcData, size_t size);
  //std::future<std::vector<uint8_t>> Download(SDL_GPUBuffer * srcBuffer, size_t offset, size_t size);

private:
  using Task = std::pair<SDL_GPUTransferBufferLocation, SDL_GPUBufferRegion>;
  std::vector<Task> m_uploadTasks;

private:
  void FreeTransferBuffers();
};
} // namespace GameFramework
