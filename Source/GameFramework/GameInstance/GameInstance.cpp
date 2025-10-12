#include "GameInstance.hpp"

namespace GameFramework
{

GameInstance::GameInstance(const std::filesystem::path & path)
  : m_lib(path)
{
  m_lib.initGameFunc();
  m_game = m_lib.getGameHandleFunc();
  m_gameName = m_lib.getGameNameFunc();
}

GameInstance::~GameInstance()
{
  m_lib.terminateGameFunc();
}

void GameInstance::Run()
{
  m_running.store(true);
  m_thread = std::thread{&GameInstance::GameThreadMain, this};
}

void GameInstance::Shutdown()
{
  m_running.store(false);
  m_thread.join();
}

const std::string & GameInstance::GetGameName() const & noexcept
{
  return m_gameName;
}

void GameInstance::GameThreadMain()
{
  std::chrono::steady_clock::time_point end, start{std::chrono::steady_clock::now()};
  while (m_running)
  {
    end = std::chrono::steady_clock::now();
    float delta = std::chrono::duration<float, std::milli>(end - start).count();
    start = end;
    m_lib.tickGameFunc(delta);
  }
}


} // namespace GameFramework
