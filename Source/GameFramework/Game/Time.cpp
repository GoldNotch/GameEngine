#include "Time.hpp"

#include <atomic>
#include <chrono>

namespace GameFramework
{

class ChronoTimeManager final : public TimeManager
{
  using Clock = std::chrono::steady_clock;
  using Duration = std::chrono::duration<double, std::milli>;
  using TimePoint = std::chrono::time_point<Clock, Duration>;

public:
  virtual void Tick() override;
  virtual double Now() const noexcept override;

private:
  const TimePoint m_appStart = Clock::now(); // time of start application
  TimePoint m_lastTime = m_appStart;
  TimePoint m_currentTime = m_appStart;

  std::atomic<double> m_cachedNow = 0.0;
};

void ChronoTimeManager::Tick()
{
  m_lastTime = m_currentTime;
  m_currentTime = Clock::now();
  m_cachedNow = (m_currentTime - m_appStart).count();
}

double ChronoTimeManager::Now() const noexcept
{
  return m_cachedNow;
}


static ChronoTimeManager g_timeManager;

GAME_FRAMEWORK_API TimeManager & GetTimeManager()
{
  return g_timeManager;
}
} // namespace GameFramework
