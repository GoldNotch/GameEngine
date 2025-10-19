#include "Signal.hpp"

#include <cstdint>

#include <concurrent_priority_queue.h>


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

} // namespace GameFramework
