#pragma once
#include <GameFramework_def.h>

#include <optional>
#include <span>
#include <vector>

#include <Input/Input.hpp>

namespace GameFramework
{

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

  void BindInputQueue(GameFramework::InputQueue & queue);
  void PushInputEvent(const GameInputEvent & event);

private:
  details::InputsQueueList * m_boundInputs = nullptr;
};

struct GAME_FRAMEWORK_API InputConsumer
{
  InputConsumer();
  virtual ~InputConsumer();
  std::optional<GameInputEvent> ConsumeInputEvent();
  void ListenInputQueue(GameFramework::InputQueue & queue);

private:
  details::InputsQueueList * m_listenedInputs = nullptr;
};

} // namespace GameFramework
