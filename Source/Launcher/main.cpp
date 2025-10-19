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
  auto * gameInstance = dynamic_cast<GameFramework::GamePLugin *>(gamePluginLoader->GetInstance());

  GameFramework::InputQueue input;
  GameFramework::SignalsQueue signalsQueue;

  Utils::GlfwInstance instance;
  std::vector<WindowUPtr> windows;
  for (auto && wndInfo : gameInstance->GetOutputConfiguration())
  {
    auto && wnd =
      windows.emplace_back(Utils::NewWindow(wndInfo.title, wndInfo.width, wndInfo.height));
    wnd->BindInputQueue(input);
  }
  gameInstance->ListenInputQueue(input);
  gameInstance->BindSignalsQueue(signalsQueue);

  while (std::all_of(windows.begin(), windows.end(),
                     [](const WindowUPtr & wnd) { return !wnd->ShouldClose(); }))
  {
    instance.PollEvents();
    for (auto && wnd : windows)
      wnd->GenerateInputEvents();
    gameInstance->Tick(0.0f);
  }

  return 0;
}
