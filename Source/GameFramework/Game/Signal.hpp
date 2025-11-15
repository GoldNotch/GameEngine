#pragma once
#include <GameFramework_def.h>

#include <optional>

namespace GameFramework
{
/// priority of signal is coded in number
/// 100 - 199 - first priority (high)
/// 200 - 299 - second priority (major)
/// 300 - 399 - third priority (minor)
enum class GameSignal : uint32_t
{
  Quit = 200,
  InvalidateRenderCache = 100,
  UpdateInputConfiguration = 101,
};

struct GAME_FRAMEWORK_API SignalsQueue final
{
  SignalsQueue();
  ~SignalsQueue();
  void PushSignal(const GameSignal & signal);
  std::optional<GameSignal> PopSignal();

private:
  struct Queue;
  Queue * m_queue = nullptr;
};

namespace details
{
struct SignalsQueueList;
}

struct GAME_FRAMEWORK_API SignalsConsumer
{
  SignalsConsumer();
  virtual ~SignalsConsumer();
  virtual void ListenSignalsQueue(GameFramework::SignalsQueue & queue);
  virtual std::optional<GameSignal> ConsumeSignal();

private:
  details::SignalsQueueList * m_listenedQueues = nullptr;
};

struct GAME_FRAMEWORK_API SignalsProducer
{
  SignalsProducer();
  virtual ~SignalsProducer();
  void BindSignalsQueue(GameFramework::SignalsQueue & queue);
  void GenerateSignal(const GameSignal & signal);

private:
  details::SignalsQueueList * m_boundQueues = nullptr;
};


} // namespace GameFramework
