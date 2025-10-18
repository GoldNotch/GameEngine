#pragma once
#include <atomic>
#include <stdexcept>
#include <thread>

#include <GameInstance/GameLibrary.hpp>

namespace GameFramework
{

struct GameInstance final
{
  explicit GameInstance(const std::filesystem::path & path);
  ~GameInstance();
  /// launches game thread and game is began
  void Run();
  /// Stops game thread
  void Shutdown();
  /// Get name of the game
  std::string GetGameName() const;

private:
  std::atomic_bool m_running = false;
  GameLibrary m_lib;
  std::thread m_thread;

private:
  void GameThreadMain();
};

} // namespace GameFramework
