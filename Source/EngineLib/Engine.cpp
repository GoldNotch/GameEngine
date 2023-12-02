#include "Engine.h"

#include <vector>
#include <memory>
#include <ctime>

inline color_t RGB2UINT(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	return (a << 24u) + (b << 16u) + (g << 8u) + r;
}

struct USEngine
{
	std::vector<color_t> result_image;
	usVideoOptions current_opts = {0, 0};
};

static std::unique_ptr<USEngine> st_engine = nullptr;

USENGINE_API bool usEngineInit(usConstructOptions options)
{
	srand(time(NULL));
	try {
		st_engine = std::make_unique<USEngine>();
	}
	catch (...)
	{
		return false;
	}
	return true;
}


USENGINE_API void usBeginFrame(usVideoOptions opts)
{
	st_engine->result_image.reserve(opts.width * opts.height);
	st_engine->current_opts = opts;
}


USENGINE_API Frame usWaitForResult()
{
	auto&& opts = st_engine->current_opts;
	for (size_t i = 0; i < opts.width * opts.height; ++i)
	{
		st_engine->result_image.push_back(RGB2UINT(rand() % 256, rand() % 256, rand() % 256, 0xFF));
	}
	return Frame{ st_engine->result_image.data(),  opts};
}


USENGINE_API void usEndFrame()
{
	st_engine->result_image.clear();
}


USENGINE_API void usEngineTerminate()
{
	st_engine.reset();
}