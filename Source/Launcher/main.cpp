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
  if (argc < 4)
  {
    std::printf("Incorrect launch format");
    return -1;
  }
  std::unique_ptr<GameFramework::IPluginLoader> gamePlugin;
  std::unique_ptr<GameFramework::IPluginLoader> windowsPlugin;
  std::unique_ptr<GameFramework::IPluginLoader> renderPlugin;

  std::filesystem::path gamePath = argv[1];
  std::filesystem::path windowsPluginPath = argv[2];
  std::filesystem::path renderPluginPath = argv[3];
  try
  {
    gamePlugin = GameFramework::LoadPlugin(gamePath);
    windowsPlugin = GameFramework::LoadPlugin(windowsPluginPath);
    renderPlugin = GameFramework::LoadPlugin(renderPluginPath);
  }
  catch (const std::exception & e)
  {
    std::printf("%s", e.what());
    return -1;
  }
  auto * gameInstance = dynamic_cast<GameFramework::GamePlugin *>(gamePlugin->GetInstance());
  auto * windowsManager =
    dynamic_cast<GameFramework::WindowsPlugin *>(windowsPlugin->GetInstance());
  auto * renderManager = dynamic_cast<GameFramework::RenderPlugin *>(renderPlugin->GetInstance());

  GameFramework::InputQueue input;
  GameFramework::SignalsQueue signalsQueue;

  std::list<GameFramework::WindowUPtr> windows;
  std::list<GameFramework::ScreenDeviceUPtr> drawDevices;
  std::list<GameFramework::InputControllerUPtr> inputControllers;
  for (auto && wndInfo : gameInstance->GetOutputConfiguration())
  {
    using namespace GameFramework;
    auto && wnd = windows.emplace_back(
      windowsManager->NewWindow(wndInfo.id, wndInfo.title, wndInfo.width, wndInfo.height));
    auto && controller =
      inputControllers.emplace_back(GameFramework::CreateInputController(wnd->GetInput()));
    controller->BindInputQueue(input);
    auto && device = drawDevices.emplace_back(renderManager->CreateScreenDevice(*wnd));
    wnd->SetResizeCallback([dcPtr = device.get()](int w, int h) { dcPtr->OnResize(w, h); });
  }
  gameInstance->ListenInputQueue(input);
  gameInstance->BindSignalsQueue(signalsQueue);

  // in the beginning we must read and update input configuration
  signalsQueue.PushSignal(GameFramework::GameSignal::UpdateInputConfiguration);

  while (std::all_of(windows.begin(), windows.end(),
                     [](const GameFramework::WindowUPtr & wnd) { return !wnd->ShouldClose(); }))
  {
    GameFramework::GetTimeManager().Tick();
    windowsManager->PollEvents();
    for (auto && controller : inputControllers)
      controller->GenerateInputEvents();
    gameInstance->ProcessInput();

    renderManager->Tick();

    for (auto && dc : drawDevices)
    {
      if (dc && dc->BeginFrame())
      {
        gameInstance->Render(*dc);
        dc->EndFrame();
      }
    }

    while (auto signal = signalsQueue.PopSignal())
    {
      switch (signal.value())
      {
        case GameFramework::GameSignal::UpdateInputConfiguration:
        {
          auto conf = gameInstance->GetInputConfiguration();
          for (auto && controller : inputControllers)
            controller->SetInputBindings(conf);
        }
        break;
        case GameFramework::GameSignal::Quit:
          return 0;
        case GameFramework::GameSignal::InvalidateRenderCache:
        {
          for (auto && dc : drawDevices)
            dc->Refresh();
        }
        break;
      }
    }

    gameInstance->Tick(GameFramework::GetTimeManager().Delta());
  }

  return 0;
}
