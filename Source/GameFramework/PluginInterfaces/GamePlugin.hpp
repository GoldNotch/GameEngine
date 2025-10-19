/// It's functions which should be implemented in dll for Game Framework detected it as Game
#pragma once
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include <Game/Input.hpp>
#include <Game/Signal.hpp>
#include <Plugin/Plugin.hpp>
#include <PluginInterfaces/DrawToolPlugin.hpp>

namespace GameFramework
{

struct ProtoGameAction
{
  std::string name;     ///< name of the action, can be "Jump", "MoveForward", etc
  int code;             ///< code ID of the action, use enum to define it
  std::string bindings; ///< string to declare keys that represents this action
};

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
  virtual std::vector<ProtoGameAction> GetInputConfiguration() const = 0;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const = 0;

  virtual void Tick(float deltaTime) = 0;
  virtual void Render(GameFramework::IDrawTool & drawTool) = 0;
};

} // namespace GameFramework
