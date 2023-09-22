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
#include <chrono>
#include "Window.hpp"

extern "C"
{
#include <GL/glew.h>
#include <glfw/glfw3.h>
}

using namespace Framework;

static void UpdateViewport(GLFWwindow* window)
{
	/*int fm_width, fm_height;
	glfwGetFramebufferSize(window, &fm_width, &fm_height);
	glViewport(0, 0, fm_width, fm_height);*/
}

static void key_callback(GLFWwindow* handler, int key, int scancode, int action, int mods)
{
	OSWindow* window = reinterpret_cast<OSWindow*>(glfwGetWindowUserPointer(handler));
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
}

static void window_resize_callback(GLFWwindow* window, int width, int height)
{
	UpdateViewport(window);
}

std::unique_ptr<OSWindow> OSWindow::main_window = nullptr;

struct OSWindow::ImplData
{
	GLFWwindow* glfw_wnd = nullptr;
};

/// ========================= public API ========================

/// @brief move constructor
/// @param other
OSWindow::OSWindow(OSWindow&& other) noexcept
	: impl_data(std::move(other.impl_data)),
	child_windows(std::move(other.child_windows))
{
}

/// @brief move operator
/// @param other
/// @return
OSWindow& OSWindow::operator=(OSWindow&& other) noexcept
{
	impl_data = std::move(other.impl_data);
	child_windows = std::move(other.child_windows);
	return *this;
}

/// @brief destructor
OSWindow::~OSWindow()
{
	child_windows.clear();
	glfwSetWindowShouldClose(impl_data->glfw_wnd, GLFW_TRUE);
	glfwDestroyWindow(impl_data->glfw_wnd);
	if (rendering_thread)
		rendering_thread->join();
	if (this == OSWindow::main_window.get())
		glfwTerminate();
}

/// @brief creates child window
/// @param width
/// @param height
/// @param title
/// @return
OSWindow& OSWindow::CreateChildWindow(int width, int height, const char* title) &
{
	auto& wnd = child_windows.emplace_back(width, height, title);
	return wnd;
}

/// @brief check is window should close
/// @return true if window is should close, false otherwise
bool OSWindow::IsClosing() const noexcept
{
	return glfwWindowShouldClose(impl_data->glfw_wnd);
}

void OSWindow::Close() noexcept
{
	for (auto&& child_wnd : child_windows)
		child_wnd.Close();
	glfwSetWindowShouldClose(impl_data->glfw_wnd, true);
}

/// ======================== Static Public API ========================

/// @brief creates main window
/// @param width
/// @param height
/// @param title
/// @return
OSWindow& OSWindow::CreateMainWindow(int width, int height, const char* title)
{
	if (OSWindow::main_window == nullptr)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// TODO: make hints setting
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		OSWindow* wnd_ptr = new OSWindow(width, height, title);
		wnd_ptr->CreateContext();
		IRenderer::InitRenderingSystem();
		OSWindow::main_window.reset(wnd_ptr);
	}

	return *OSWindow::main_window;
}

/// @brief static method to run all created windows. Block calling thread
void OSWindow::RunWindows() noexcept
{
	auto& main_wnd = GetMainWindow();
	main_wnd.RunChildWindowsLoops(true /*is_main_window*/);
	while (!main_wnd.IsClosing())
	{
		glfwPollEvents();
		// if (main_wnd.renderer)
		main_wnd.RenderPass(true /*is_main_window*/);
	}
}

/// ========================== Private functions =======================

/// @brief Create OpenGL context for calling thread (maybe other contexts)
void OSWindow::CreateContext() noexcept
{
	glfwMakeContextCurrent(impl_data->glfw_wnd);
}

/// @brief rendering pass
void OSWindow::RenderPass(bool is_main_window) noexcept
{
	//TODO: make event-oriented mode for rendering calls (i.e. call rendering as events on user actions)
	double prerender_timestamp = glfwGetTime();
	if (renderer == nullptr)
		return;
	// if it's main window then it's main thread and we can't use sleep function
	if (is_main_window && prerender_timestamp - last_rendering_timestamp < rendering_interval)
		return;

	{
		std::scoped_lock lk{ rendering_mutex };
		// call renderer
		renderer->RenderPass(prerender_timestamp);
	}

	glfwSwapBuffers(impl_data->glfw_wnd);
	last_rendering_timestamp = glfwGetTime();
	double elapsed_time_in_sec = last_rendering_timestamp - prerender_timestamp;
	fps = 1.0 / elapsed_time_in_sec;

	// if it's child window then it uses separate thread for rendering and then we can use sleep
	if (!is_main_window)
	{
		auto sleep_dur = std::chrono::duration<double>(rendering_interval - elapsed_time_in_sec);
		std::this_thread::sleep_for(sleep_dur);
	}
}

/// @brief Service function to run rendering threads for child windows
/// @param is_main_window - flag to not launch thread
void OSWindow::RunChildWindowsLoops(bool is_main_window) noexcept
{
	InitViewport();
	for (auto&& child_wnd : child_windows)
		child_wnd.RunChildWindowsLoops(false /*is_main_window*/);
	if (!is_main_window)
	{
		rendering_thread =
			std::make_unique<std::thread>([this]
				{
					CreateContext();
					while (!IsClosing())
						RenderPass(false /*is_main_window*/); });
	}
}

/// @brief private constructor
/// @param width
/// @param height
/// @param title
OSWindow::OSWindow(int width, int height, const char* title)
{
	GLFWwindow* wnd = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (wnd == nullptr)
	{
		const char* msg;
		glfwGetError(&msg);
		std::printf("Failed to create GLFW window - %s", msg);
	}
	glfwSetWindowUserPointer(wnd, this);

	glfwSetKeyCallback(wnd, key_callback);
	glfwSetCursorPosCallback(wnd, cursor_position_callback);
	glfwSetMouseButtonCallback(wnd, mouse_button_callback);
	glfwSetScrollCallback(wnd, scroll_callback);
	glfwSetWindowSizeCallback(wnd, window_resize_callback);
	glfwSetKeyCallback(wnd, key_callback);
	impl_data = std::make_unique<ImplData>();
	impl_data->glfw_wnd = wnd;
}

/// @brief Initialize openGL viewport
void OSWindow::InitViewport() noexcept
{
	UpdateViewport(impl_data->glfw_wnd);
}
