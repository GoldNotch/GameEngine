/// It's functions which should be implemented in dll for Game Framework detected it as Game
#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <Game/Signal.hpp>
#include <Input/Input.hpp>
#include <Input/InputQueue.hpp>
#include <Plugin/Plugin.hpp>
#include <PluginInterfaces/RenderPlugin.hpp>

namespace GameFramework
{

struct ProtoWindow
{
  int id; ///< user-defined id. Declare enum for that
  std::string title;
  int width;
  int height;
};

struct GAME_FRAMEWORK_API GamePlugin : public IPluginInstance,
                                       public InputConsumer,
                                       public SignalsProducer
{
  virtual ~GamePlugin() = default;
  virtual std::string GetGameName() const = 0;
  virtual std::vector<InputBinding> GetInputConfiguration() const = 0;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const = 0;

  virtual void Tick(double deltaTime) = 0;
  virtual void Render(GameFramework::IDevice & device) = 0;
  void ProcessInput();

protected:
  virtual void OnAction(const EventAction & action) {};
  virtual void OnAction(const ContinousAction & action) {};
  virtual void OnAction(const AxisAction & action) {};
};

} // namespace GameFramework
