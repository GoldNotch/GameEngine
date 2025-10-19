#pragma once
#include <GameFramework_def.h>

#include <optional>
#include <span>
#include <string>
#include <variant>
#include <vector>

namespace GameFramework
{

struct GAME_FRAMEWORK_API InputBinding
{
  char name[128];      ///< name of the action, can be "Jump", "MoveForward", etc
  int code;            ///< code ID of the action, use enum to define it
  char bindings[1024]; ///< string to declare keys that represents this action
};

struct GAME_FRAMEWORK_API InputAction
{
  int code = 0;           ///< code ID of the action, use enum to define it
  bool isPressed = false; ///< true if action is pressed or active
};

struct GAME_FRAMEWORK_API InputAxis
{
  int code = 0; ///< code ID of the action, use enum to define it
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
  Queue * m_queue = nullptr;
};

namespace details
{
struct InputsQueueList;
}

struct GAME_FRAMEWORK_API InputProducer
{
  InputProducer();
  virtual ~InputProducer();
  virtual void GenerateInputEvents() = 0;
  virtual void SetInputBindings(const std::span<InputBinding> & bindings) = 0;

public:
  void BindInputQueue(GameFramework::InputQueue & queue);
  void PushInputEvent(const GameInputEvent & event);

private:
  details::InputsQueueList * m_boundInputs = nullptr;
};

struct GAME_FRAMEWORK_API InputConsumer
{
  InputConsumer();
  virtual ~InputConsumer();
  virtual void ProcessInputEvents() = 0;
  virtual std::vector<InputBinding> GetInputConfiguration() const = 0;

public:
  std::optional<GameInputEvent> ConsumeInputEvent();
  void ListenInputQueue(GameFramework::InputQueue & queue);

private:
  details::InputsQueueList * m_listenedInputs = nullptr;
};

} // namespace GameFramework
