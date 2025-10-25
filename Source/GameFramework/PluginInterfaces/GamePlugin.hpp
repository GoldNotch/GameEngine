/// It's functions which should be implemented in dll for Game Framework detected it as Game
#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <Game/Signal.hpp>
#include <Input/InputQueue.hpp>
#include <Plugin/Plugin.hpp>
#include <PluginInterfaces/DrawToolPlugin.hpp>

namespace GameFramework
{

struct ProtoWindow
{
  std::string title;
  int width;
  int height;
};

struct GAME_FRAMEWORK_API GamePLugin : public IPluginInstance,
                                       public InputConsumer,
                                       public SignalsProducer
{
  virtual ~GamePLugin() = default;
  virtual std::string GetGameName() const = 0;
  virtual std::vector<InputBinding> GetInputConfiguration() const = 0;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const = 0;

  virtual void Tick(double deltaTime) = 0;
  virtual void Render(GameFramework::IDrawTool & drawTool) = 0;
};

} // namespace GameFramework
