#include "Input.hpp"

#include <concurrent_queue.h>
namespace GameFramework
{


struct InputQueue::Queue final
{
  Concurrency::concurrent_queue<GameInputEvent> impl;
};

InputQueue::InputQueue()
  : m_queue(new InputQueue::Queue())
{
}

InputQueue::~InputQueue()
{
  delete m_queue;
}

void InputQueue::PushEvent(const GameInputEvent & event)
{
  m_queue->impl.push(event);
}

std::optional<GameInputEvent> InputQueue::PopEvent()
{
  GameInputEvent result;
  return m_queue->impl.try_pop(result) ? std::make_optional(result) : std::nullopt;
}

} // namespace GameFramework
