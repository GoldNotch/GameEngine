#include "Process.hpp"
#include <cassert>
#include <App.h>
namespace App
{

void MainProcess::Start()
{
  app_thread = std::make_unique<std::thread>(&MainProcess::AppMain, this);
}

void MainProcess::Terminate()
{
  if (IsStarted())
  {
    assert(!is_paused);
    is_should_terminate = true;
    app_thread->join();
    app_thread.reset();
  }
}

void MainProcess::Pause()
{
  tick_lock.lock();
  is_paused = true;
}

void MainProcess::Resume()
{
  assert(is_paused);
  tick_lock.unlock();
  is_paused = false;
}

void MainProcess::AppMain()
{
  usApp_Init();
  while (!is_should_terminate)
  {
    std::scoped_lock lock{this->tick_lock};
    usApp_Tick();
  }
  usApp_Terminate();
}

} // namespace App
