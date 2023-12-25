#pragma once
#include <string>
#include <utility>

#include "../Engine.h"

#ifdef LOGGING_IMPLEMENTATION
namespace io
{
namespace p_detail
{
usLogFunc __st_logFunc__ = nullptr;
thread_local std::string buffer(1024, '\0');
} // namespace p_detail

} // namespace io
USENGINE_API void usSetLoggingFunc(usLogFunc func)
{
  io::p_detail::__st_logFunc__ = func;
}
#else
namespace io
{
namespace p_detail
{
extern usLogFunc __st_logFunc__;
extern thread_local std::string buffer;
} // namespace p_detail
} // namespace io
#endif

namespace io
{
template<typename... Args>
void Log(usLogStatus status, int code, const char * format, Args &&... args)
{
  if (p_detail::__st_logFunc__)
  {
    std::sprintf(const_cast<char *>(p_detail::buffer.c_str()), format, args...);
    p_detail::__st_logFunc__(status, code, p_detail::buffer.c_str());
  }
}
} // namespace io
