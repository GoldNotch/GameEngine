#pragma once
#include <filesystem>
#include <memory>

#include <GameFramework.hpp>

namespace dylib
{
class library;
}

namespace GameFramework
{

class GameLibrary final
{
  std::unique_ptr<dylib::library> m_sharedLibrary;

public:
  GetGameNameFunc * getGameNameFunc = nullptr;
  GetInputConfigurationFunc * getInputConfigurationFunc = nullptr;
  GetOutputConfigurationFunc * getOutputConfigurationFunc = nullptr;
  InitGameFunc * initGameFunc = nullptr;
  TickGameFunc * tickGameFunc = nullptr;
  RenderGameFunc * renderGameFunc = nullptr;
  ProcessInputFunc * processInputFunc = nullptr;
  TerminateGameFunc * terminateGameFunc = nullptr;

  explicit GameLibrary(const std::filesystem::path & path);
  ~GameLibrary();
};

} // namespace GameFramework
