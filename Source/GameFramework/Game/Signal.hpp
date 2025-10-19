#pragma once
#include <GameFramework_def.h>

#include <optional>

namespace GameFramework
{
/// priority of signal is coded in number
/// 100 - 199 - first priority (high)
/// 200 - 299 - second priority (major)
/// 300 - 399 - third priority (minor)
enum GameSignal
{
  Quit = 200,
  InvalidateRenderCache = 100
};

struct GAME_FRAMEWORK_API SignalsQueue final
{
  SignalsQueue();
  ~SignalsQueue();
  void PushSignal(const GameSignal & signal);
  std::optional<GameSignal> PopSignal();

private:
  struct Queue;
  Queue * m_queue;
};

struct GAME_FRAMEWORK_API SignalsConsumer
{
  virtual ~SignalsConsumer() = default;
  virtual void ListenSignalsQueue(GameFramework::SignalsQueue & queue) = 0;
  virtual void ProcessSignals() = 0;
};

struct GAME_FRAMEWORK_API SignalsProducer
{
  virtual ~SignalsProducer() = default;
  virtual void BindSignalsQueue(GameFramework::SignalsQueue & queue) = 0;
  virtual void GenerateSignal(const GameSignal & signal) = 0;
};


} // namespace GameFramework
