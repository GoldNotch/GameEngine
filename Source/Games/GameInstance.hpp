#pragma once
#include <atomic>
#include <stdexcept>
#include <thread>

#include "GameLibrary.hpp"

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
  const std::string & GetGameName() const & noexcept;

  IGame & GetGame() & noexcept;

private:
  std::atomic_bool m_running = false;
  GameLibrary m_lib;
  IGame * m_game;
  std::string m_gameName;
  std::thread m_thread;

private:
  void GameThreadMain();
};

} // namespace GameFramework
