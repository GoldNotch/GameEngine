/// It's functions which should be implemented in dll for Game Framework detected it as Game
#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <variant>
#include <vector>

#include <DrawTool.hpp>
#include <Plugin/Plugin.hpp>

namespace GameFramework
{

struct QuitSignal
{
};

struct InvalidateRenderCacheSignal
{
};

struct GameAction
{
  std::string name;       ///< name of the action, can be "Jump", "MoveForward", etc
  int code;               ///< code ID of the action, use enum to define it
  bool isPressed = false; ///< true if action is pressed or active
  float xAxisValue = 0.0f;
  float dxAxisValue = 0.0f;
  float yAxisValue = 0.0f;
  float dyAxisValue = 0.0f;
};

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

using GameSignal = std::variant<QuitSignal, InvalidateRenderCacheSignal>;

struct BaseGame : public IPluginInstance
{
  virtual ~BaseGame() = default;
  virtual std::string GetGameName() const = 0;
  virtual std::vector<ProtoGameAction> GetInputConfiguration() const = 0;
  virtual std::vector<ProtoWindow> GetOutputConfiguration() const = 0;

  virtual void Tick(float deltaTime) = 0;
  virtual void Render(GameFramework::IDrawTool & drawTool) = 0;

public:
  void PushSignal(const GameSignal & signal);
  GameSignal PollSignal() noexcept;

private:
  std::mutex m;
  std::queue<GameSignal> m_signals;
};

} // namespace GameFramework
