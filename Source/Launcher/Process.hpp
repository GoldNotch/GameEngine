#pragma once
#include <mutex>
#include <thread>
#include <type_traits>
#include <utility>
namespace App
{
/// @brief main process for application. launches application thread
struct MainProcess final
{
  MainProcess() = default;
  ~MainProcess() noexcept { Terminate(); }

  /// @brief Start application process
  void Start();
  /// @brief Terminate application process. Be sure if ticks didn't pause
  void Terminate();
  /// @brief pause ticks in application until resume is called
  void Pause();
  /// @brief resume ticks in application
  void Resume();
  /// @brief check if application process is started
  bool IsStarted() const noexcept { return app_thread != nullptr; }

  /// @brief pauses ticks loop and executes some function
  template<typename Fn, typename... Args>
  std::invoke_result_t<Fn, Args...> ExecuteWithPause(Fn && func, Args &&... args)
  {
    std::scoped_lock lk{tick_lock};
    return func(std::forward<decltype(args)>(args)...);
  }

private:
  std::unique_ptr<std::thread> app_thread = nullptr; ///< app thread, process scene
  // communication queue
  std::mutex tick_lock;                         ///< mutex to pause ticks
  std::atomic_bool is_should_terminate = false; ///< flag to terminate process
  bool is_paused = false;                       ///< flag to check if process is paused

private:
  MainProcess(const MainProcess &) = delete;
  MainProcess & operator=(const MainProcess &) = delete;
  /// @brief main function for application process
  void AppMain();
};

} // namespace App
