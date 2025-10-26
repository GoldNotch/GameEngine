#include <cstdio>
#include <filesystem>
#include <list>
#include <memory>
#include <stdexcept>

#include <Game/Time.hpp>
#include <GameFramework.hpp>

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
  if (argc < 3)
  {
    std::printf("Incorrect launch format");
    return -1;
  }
  std::filesystem::path gamePath = argv[1];
  std::filesystem::path windowsPluginPath = argv[2];
  auto gamePluginLoader = GameFramework::LoadPlugin(gamePath);
  auto * gameInstance = dynamic_cast<GameFramework::GamePlugin *>(gamePluginLoader->GetInstance());
  auto windowsPluginLoader = GameFramework::LoadPlugin(windowsPluginPath);
  auto * windowsPlugin =
    dynamic_cast<GameFramework::WindowsPlugin *>(windowsPluginLoader->GetInstance());

  GameFramework::InputQueue input;
  GameFramework::SignalsQueue signalsQueue;

  std::vector<GameFramework::WindowUPtr> windows;
  for (auto && wndInfo : gameInstance->GetOutputConfiguration())
  {
    auto && wnd =
      windows.emplace_back(windowsPlugin->NewWindow(wndInfo.title, wndInfo.width, wndInfo.height));
    wnd->GetInputController().BindInputQueue(input);
  }
  gameInstance->ListenInputQueue(input);
  gameInstance->BindSignalsQueue(signalsQueue);

  // in the beginning we must read and update input configuration
  signalsQueue.PushSignal(GameFramework::GameSignal::UpdateInputConfiguration);

  while (std::all_of(windows.begin(), windows.end(),
                     [](const GameFramework::WindowUPtr & wnd) { return !wnd->ShouldClose(); }))
  {
    GameFramework::GetTimeManager().Tick();
    windowsPlugin->PollEvents();
    for (auto && wnd : windows)
    {
      wnd->GetInputController().GenerateInputEvents();
      //TODO: render in window
    }

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

    gameInstance->Tick(GameFramework::GetTimeManager().Delta());
  }

  return 0;
}
