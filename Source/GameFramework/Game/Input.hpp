#pragma once
#include <GameFramework_def.h>

#include <optional>
#include <variant>

namespace GameFramework
{
struct GAME_FRAMEWORK_API InputAction
{
  int code;               ///< code ID of the action, use enum to define it
  bool isPressed = false; ///< true if action is pressed or active
};

struct GAME_FRAMEWORK_API InputAxis
{
  int code; ///< code ID of the action, use enum to define it
  float xAxisValue = 0.0f;
  float dxAxisValue = 0.0f;
  float yAxisValue = 0.0f;
  float dyAxisValue = 0.0f;
};

using GameInputEvent = std::variant<InputAction, InputAxis>;

struct GAME_FRAMEWORK_API InputQueue final
{
  InputQueue();
  ~InputQueue();
  void PushEvent(const GameInputEvent & signal);
  std::optional<GameInputEvent> PopEvent();

private:
  struct Queue;
  Queue * m_queue;
};


struct GAME_FRAMEWORK_API InputProducer
{
  virtual ~InputProducer() = default;
  virtual void BindInputQueue(GameFramework::InputQueue & queue) = 0;
  virtual void GenerateInputEvents() = 0;
};

struct GAME_FRAMEWORK_API InputConsumer
{
  virtual ~InputConsumer() = default;
  virtual void ListenInputQueue(GameFramework::InputQueue & queue) = 0;
  virtual std::optional<GameInputEvent> ConsumeEvent() = 0;
};

} // namespace GameFramework
