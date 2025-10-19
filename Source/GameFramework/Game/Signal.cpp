#include "Signal.hpp"

#include <cstdint>
#include <vector>

#include <concurrent_priority_queue.h>

#include "Signal.hpp"


namespace GameFramework
{
constexpr uint32_t GetSignalPriority(GameSignal s) noexcept
{
  return static_cast<uint32_t>(s) / 100;
}

struct SignalsQueue::Queue final
{
  Concurrency::concurrent_priority_queue<GameSignal> impl;
};

SignalsQueue::SignalsQueue()
  : m_queue(new SignalsQueue::Queue())
{
}

SignalsQueue::~SignalsQueue()
{
  delete m_queue;
}

void SignalsQueue::PushSignal(const GameSignal & signal)
{
  m_queue->impl.push(signal);
}

std::optional<GameSignal> SignalsQueue::PopSignal()
{
  GameSignal result;
  return m_queue->impl.try_pop(result) ? std::make_optional(result) : std::nullopt;
}

struct details::SignalsQueueList final
{
  std::vector<SignalsQueue *> impl;
};

SignalsConsumer::SignalsConsumer()
  : m_listenedQueues(new details::SignalsQueueList)
{
}

SignalsConsumer::~SignalsConsumer()
{
  delete m_listenedQueues;
}

void SignalsConsumer::ListenSignalsQueue(GameFramework::SignalsQueue & queue)
{
  m_listenedQueues->impl.push_back(&queue);
}

std::optional<GameSignal> SignalsConsumer::ConsumeSignal()
{
  for (auto * queue : m_listenedQueues->impl)
  {
    auto result = queue->PopSignal();
    if (result.has_value())
      return result;
  }
  return std::nullopt;
}

SignalsProducer::SignalsProducer()
  : m_boundQueues(new details::SignalsQueueList)
{
}

SignalsProducer::~SignalsProducer()
{
  delete m_boundQueues;
}

void SignalsProducer::BindSignalsQueue(GameFramework::SignalsQueue & queue)
{
  m_boundQueues->impl.push_back(&queue);
}

void SignalsProducer::GenerateSignal(const GameSignal & signal)
{
  for (auto * queue : m_boundQueues->impl)
    queue->PushSignal(signal);
}

} // namespace GameFramework
