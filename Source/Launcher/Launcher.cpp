#include "Launcher.hpp"

#include <cstdio>
#include <memory>
#include <stdexcept>

#include <GameFramework.hpp>
#include <Games/GameInstance.hpp>

#include "SDL/Window.hpp"

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static std::unique_ptr<GameFramework::GameInstance> s_gameInstance;
static std::unique_ptr<GameFramework::Window_SDL> s_gameWindow;

SDL_AppResult SDL_AppInit(void ** appstate, int argc, char ** argv)
{
  if (argc < 2)
  {
    GameFramework::Log(GameFramework::Error, "Not enought params");
    return SDL_APP_FAILURE;
  }
  std::filesystem::path libPath(argv[1]);
  s_gameInstance = std::make_unique<GameFramework::GameInstance>(libPath);
  s_gameWindow =
    std::make_unique<GameFramework::Window_SDL>(s_gameInstance->GetGameName().c_str(), 1024, 768);
  s_gameWindow->BindGameInstance(s_gameInstance.get());
  s_gameInstance->Run();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void * appstate)
{
  s_gameWindow->RenderFrame();
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void * appstate, SDL_Event * event)
{
  // close the window on request
  if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
  {
    s_gameInstance->Shutdown();
    s_gameWindow.reset();
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void * appstate, SDL_AppResult result)
{
  s_gameInstance.reset();
}
