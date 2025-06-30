#pragma once
#include <filesystem>
#include <memory>

#include <GameFramework.hpp>

class dylib;

namespace GameFramework
{

class GameLibrary final
{
  std::unique_ptr<dylib> m_sharedLibrary;

public:
  // This signature is taken from GameFramework.hpp
  typedef void (*InitGameFunc)();
  typedef void (*TerminateGameFunc)();
  typedef IGame * (*GetGameHandleFunc)();
  typedef const char * (*GetGameNameFunc)();

  const InitGameFunc initGameFunc = nullptr;
  const TerminateGameFunc terminateGameFunc = nullptr;
  const GetGameHandleFunc getGameHandleFunc = nullptr;
  const GetGameNameFunc getGameNameFunc = nullptr;

  explicit GameLibrary(const std::filesystem::path & path);
  ~GameLibrary();
};

} // namespace GameFramework
