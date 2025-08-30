#pragma once
#include <GameFramework_def.h>

#include <chrono>
#include <filesystem>


namespace GameFramework
{
struct GAME_FRAMEWORK_API IResource
{
  virtual ~IResource() = default;
  /// Get path to the resource on disk
  virtual const std::filesystem::path & GetPath() const & noexcept = 0;
  /// Check that data is uploaded to RAM
  virtual bool IsUploaded() const = 0;
  /// Upload resource to RAM. return 0 if data is not uploaded yet, otherwise returns hash of uploaded data
  virtual size_t Upload() = 0;
  /// Free resource from RAM
  virtual void Free() = 0;
};

GAME_FRAMEWORK_API inline size_t CalcResourceTimestamp()
{
  // Get current time since epoch in seconds
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
  return static_cast<size_t>(seconds);
}

template<typename ResourceT, typename... Args>
std::unique_ptr<ResourceT> ConstructResource(Args &&... args);

} // namespace GameFramework
