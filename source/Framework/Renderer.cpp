// Copyright 2023 JohnCorn
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "Renderer.hpp"
#include <atomic>

namespace Framework
{

	ContextID RequestNewContextID()
	{
		static std::atomic<ContextID> last_context_id = 0;
		return last_context_id++;
	}


	void IRenderer::PushNewTickData(TickID tick_id)
	{
		{
			std::unique_lock<std::mutex> lk{ mutex };
			cv_can_render.wait(lk,
				[this] {
					return drawing_tick_id - last_drawn_tick_id <= max_difference_between_ticks_and_frames;
				});
		}

		{
			std::lock_guard<std::mutex> lk{ mutex };
			drawing_tick_id = tick_id;
		}
		cv_can_render.notify_one();
	}

	void IRenderer::BeginRender()
	{
		// renderer shouldn't draw the same frame twice (or more)
		std::unique_lock<std::mutex> lk(mutex);
		cv_can_render.wait(lk, [this] {return drawing_tick_id != last_drawn_tick_id; });
	}

	void IRenderer::EndRender()
	{
		{
			std::scoped_lock<std::mutex> lk{ mutex };
			last_drawn_tick_id = drawing_tick_id;
		}
		cv_can_render.notify_one();
	}
}