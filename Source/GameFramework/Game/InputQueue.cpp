#include "InputQueue.hpp"

#include <vector>

#include <concurrent_queue.h>

#include "Input.hpp"
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

struct details::InputsQueueList final
{
  std::vector<InputQueue *> impl;
};

InputProducer::InputProducer()
  : m_boundInputs(new details::InputsQueueList)
{
}

InputProducer::~InputProducer()
{
  delete m_boundInputs;
}

void InputProducer::BindInputQueue(InputQueue & queue)
{
  m_boundInputs->impl.push_back(&queue);
}

void InputProducer::PushInputEvent(const GameInputEvent & event)
{
  for (auto * queue : m_boundInputs->impl)
    queue->PushEvent(event);
}

InputConsumer::InputConsumer()
  : m_listenedInputs(new details::InputsQueueList)
{
}

InputConsumer::~InputConsumer()
{
  delete m_listenedInputs;
}

std::optional<GameInputEvent> InputConsumer::ConsumeInputEvent()
{
  // очень спорно
  for (auto * queue : m_listenedInputs->impl)
  {
    auto evt = queue->PopEvent();
    if (evt.has_value())
      return evt;
  }
  return std::nullopt;
}

void InputConsumer::ListenInputQueue(GameFramework::InputQueue & queue)
{
  m_listenedInputs->impl.push_back(&queue);
};

} // namespace GameFramework
