#pragma once
#include <utility>
#include "../Engine.h"

#ifdef LOGGING_IMPLEMENTATION
static usLogFunc st_logFunc = nullptr;
template<typename... Args>
void Log(Args && ...args)
{
	if (st_logFunc)
		st_logFunc(std::forward(args)...);
}

template void Log<usLogStatus, int, const char*>(usLogStatus, int, const char*);

USENGINE_API void usSetLoggingFunc(usLogFunc func)
{
	st_logFunc = func;
}
#else
extern template void Log<usLogStatus, int, const char*>(usLogStatus status, int code, const char* message);
#endif