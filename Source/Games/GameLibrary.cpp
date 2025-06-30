#include "GameLibrary.hpp"

#include <dylib.hpp>


namespace GameFramework
{

GameLibrary::GameLibrary(const std::filesystem::path & path)
  : m_sharedLibrary(std::make_unique<dylib>(path))
  , initGameFunc(m_sharedLibrary->get_function<void()>("InitGame"))
  , terminateGameFunc(m_sharedLibrary->get_function<void()>("TerminateGame"))
  , getGameHandleFunc(m_sharedLibrary->get_function<IGame *()>("GetGameHandle"))
  , getGameNameFunc(m_sharedLibrary->get_function<const char *()>("GetGameName"))
{
}

GameLibrary::~GameLibrary() = default;

} // namespace GameFramework
