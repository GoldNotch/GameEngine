#include <cstdio>
#include <filesystem>
#include <list>
#include <memory>
#include <stdexcept>

#include <Game/Time.hpp>
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

void GameThread()
{
  while (true)
  {
  }
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
    wnd->GetInputController().BindInputQueue(input);
  }
  gameInstance->ListenInputQueue(input);
  gameInstance->BindSignalsQueue(signalsQueue);

  // in the beginning we must read and update input configuration
  signalsQueue.PushSignal(GameFramework::GameSignal::UpdateInputConfiguration);

  bool quitSignal = false;
  while (std::all_of(windows.begin(), windows.end(),
                     [](const WindowUPtr & wnd) { return !wnd->ShouldClose(); }))
  {
    GameFramework::GetTimeManager().Tick();
    instance.PollEvents();
    for (auto && wnd : windows)
      wnd->GetInputController().GenerateInputEvents();

    while (auto signal = signalsQueue.PopSignal())
    {
      switch (signal.value())
      {
        case GameFramework::GameSignal::UpdateInputConfiguration:
        {
          auto conf = gameInstance->GetInputConfiguration();
          for (auto && wnd : windows)
            wnd->GetInputController().SetInputBindings(conf);
        }
        break;
        case GameFramework::GameSignal::Quit:
          return 0;
      }
    }

    gameInstance->Tick(0.0);
  }

  return 0;
}
