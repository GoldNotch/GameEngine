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
#include <glfw/glfw3.h>
}

using namespace Framework;

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
	//UpdateViewport(window);
}


class OSWindow::ImplData
{
	GLFWwindow* glfw_wnd = nullptr;
public:
	explicit ImplData(OSWindow& owner, int width, int height, const char* title)
	{
		GLFWwindow* wnd = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (wnd == nullptr)
		{
			const char* msg;
			glfwGetError(&msg);
			std::printf("Failed to create GLFW window - %s", msg);
		}
		glfwSetWindowUserPointer(wnd, &owner);

		glfwSetKeyCallback(wnd, key_callback);
		glfwSetCursorPosCallback(wnd, cursor_position_callback);
		glfwSetMouseButtonCallback(wnd, mouse_button_callback);
		glfwSetScrollCallback(wnd, scroll_callback);
		glfwSetWindowSizeCallback(wnd, window_resize_callback);
		glfwSetKeyCallback(wnd, key_callback);
	}
	ImplData(ImplData&& other) noexcept
	{
		std::swap(other.glfw_wnd, glfw_wnd);
	}
	ImplData& operator=(ImplData&& other) & noexcept
	{
		if (&other != this)
			std::swap(other.glfw_wnd, glfw_wnd);
		return *this;
	}
	~ImplData() noexcept
	{
		glfwSetWindowShouldClose(glfw_wnd, true);
		glfwDestroyWindow(glfw_wnd);
	}
private:
	ImplData(const ImplData&) = delete;
	ImplData& operator=(const ImplData&) = delete;
public:
	inline bool IsClosing() const noexcept
	{
		return glfwWindowShouldClose(glfw_wnd);
	}

};

/// ========================= public API ========================

/// @brief destructor
OSWindow::~OSWindow()
{
	if (this == OSWindow::main_window.get())
		glfwTerminate();
}

OSWindow& Framework::OSWindow::CreateChildWindow(int width, int height, const char* title)&
{
	return child_windows.emplace_back(width, height, title);
}

/// @brief check is window should close
/// @return true if window is should close, false otherwise
bool OSWindow::IsClosing() const noexcept
{
	return impl_data->IsClosing();
}

/// ======================== Static Public API ========================

/// @brief creates main window
/// @param width
/// @param height
/// @param title
/// @return
OSWindow& OSWindow::CreateMainWindow(int width, int height, const char* title)
{
	assert(OSWindow::main_window == nullptr);
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// TODO: make hints setting
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	OSWindow* wnd_ptr = new OSWindow(width, height, title);
	OSWindow::main_window.reset(wnd_ptr);

	return *OSWindow::main_window;
}

/// ========================== Private functions =======================

/// @brief Create OpenGL context for calling thread (maybe other contexts)
//void OSWindow::CreateContext()
//{
//	glfwMakeContextCurrent(impl_data->glfw_wnd);
//}

void OSWindow::EventsHandlingMain()
{
	auto& main_wnd = GetMainWindow();
	while (!main_wnd.IsClosing())
	{
		glfwPollEvents();
	}
}

/// @brief private constructor
/// @param width
/// @param height
/// @param title
OSWindow::OSWindow(int width, int height, const char* title)
{
	impl_data.reset(new ImplData(*this, width, height, title));
}
