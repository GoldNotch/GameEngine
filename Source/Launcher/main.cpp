#include <cstdio>
#include <filesystem>
#include <list>
#include <memory>
#include <stdexcept>

#include <GameFramework.hpp>

#include "GlfwInstance.hpp"
#include "IWindow.hpp"

void RenderThread()
{
  /*while (true)
  {
    if (auto renderTarget = m_fbo.BeginFrame())
    {
      m_instance->RenderFrame(drawTool);
      m_fbo.EndFrame();
    }
  }*/
}

int main(int argc, const char * argv[])
{
  if (argc < 2)
  {
    std::printf("Incorrect launch format");
    return -1;
  }
  std::filesystem::path gamePath = argv[1];
  auto gamePluginLoader = GameFramework::LoadPlugin(gamePath);
  GameFramework::BaseGame * gameInstance =
    dynamic_cast<GameFramework::BaseGame *>(gamePluginLoader->GetInstance());

  Utils::GlfwInstance instance;
  std::vector<WindowUPtr> windows;
  for (auto && wnd : gameInstance->GetOutputConfiguration())
  {
    windows.emplace_back(Utils::NewWindow(wnd.title, wnd.width, wnd.height));
    //create context for each window
  }

  while (std::all_of(windows.begin(), windows.end(),
                     [](const WindowUPtr & wnd) { return !wnd->ShouldClose(); }))
  {
    instance.PollEvents();
    //std::vector<GameAction> actions;
    //for (auto && wnd : windows)
    //  wnd->CollectActions(&actions);

    //for (auto && action : actions)
    //  game.ProcessInput(action);
  }

  return 0;
}
