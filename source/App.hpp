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
#pragma once
#include <vector>
#include <thread>
#include <memory>
#include <functional>
#include <atomic>
#include "Window.hpp"

namespace Framework
{

	struct IApp
	{
		using FinishCallback = std::function<void(int result_code)>;
		using AppFrameID = unsigned long long; ///< index of iteration

		IApp(OSWindow& window) : window(window) {}
		/// @brief destructor
		virtual ~IApp() 
		{
			is_running = false;
			if (app_thread)
				app_thread->join();
			app_thread.reset();
			window.Close();
		}
		/// @brief set callback for finish event
		/// @param on_finish_func - callback function
		void SetOnFinishCallback(FinishCallback on_finish_func) { on_finish = on_finish_func; }
		/// @brief launch app-thread
		void Run()
		{
			is_running = true;
			app_thread.reset(new std::thread([this] {
				int result = AppMain();
				if (on_finish) on_finish(result);
				is_running = false;
				}));
			if (&window == &OSWindow::GetMainWindow())
				OSWindow::RunWindows();
		}
		/// @brief check if app is running
		/// @return true if app-thread is running
		bool IsRunning() const { return is_running; }

	protected:
		/// @brief execution for app
		/// @return error code for app
		virtual int AppMain() = 0;

		/// starts new app frame
		virtual AppFrameID BeginFrame() { return current_frame_id++; }
		/// ends app frame, synchronise with renderer to pass data
		virtual void EndFrame() 
		{
			
		}

	protected:
		//output settings
		OSWindow& window; ///< window fpr app's output

	private:
		std::unique_ptr<std::thread> app_thread = nullptr; ///< thread for app
		FinishCallback on_finish = nullptr;	///< callback function called after app is closed
		std::atomic_bool is_running = false; ///< atomic flag that app is running

		AppFrameID current_frame_id = 0; ///< counter for frames
	private:
		IApp(const IApp&) = delete;
		IApp& operator=(IApp&) = delete;
	};

}


