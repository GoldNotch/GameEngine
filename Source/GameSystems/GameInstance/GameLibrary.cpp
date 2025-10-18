#include "GameLibrary.hpp"

#include <dylib.hpp>


namespace GameFramework
{

GameLibrary::GameLibrary(const std::filesystem::path & path)
  : m_sharedLibrary(std::make_unique<dylib::library>(path))
{
  getGameNameFunc = m_sharedLibrary->get_function<GetGameNameFunc>("GetGameName");
  getInputConfigurationFunc =
    m_sharedLibrary->get_function<GetInputConfigurationFunc>("GetInputConfiguration");
  getOutputConfigurationFunc =
    m_sharedLibrary->get_function<GetOutputConfigurationFunc>("GetOutputConfiguration");
  renderGameFunc = m_sharedLibrary->get_function<RenderGameFunc>("RenderGame(GameFramework::IDrawTool &)");
  processInputFunc = m_sharedLibrary->get_function<ProcessInputFunc>("ProcessInput(GameAction const &)");
  initGameFunc = m_sharedLibrary->get_function<InitGameFunc>("InitGame");
  terminateGameFunc = m_sharedLibrary->get_function<TerminateGameFunc>("TerminateGame");
  tickGameFunc = m_sharedLibrary->get_function<TickGameFunc>("TickGame(float)");
}

GameLibrary::~GameLibrary() = default;

} // namespace GameFramework
