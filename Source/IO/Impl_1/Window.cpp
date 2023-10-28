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
#include <Window.h>
extern "C"
{
#include <glfw/glfw3.h>
}

#include <utility>
#include <memory>
#include <list>
#include <cassert>

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

/// RAII class to own pointer on glfw window
struct OSWindowImpl final
{
	OSWindowImpl(int width, int height, const char* title)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		// TODO: make hints setting
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		glfw_wnd = glfwCreateWindow(width, height, title, nullptr, nullptr);
		if (glfw_wnd == nullptr)
		{
			const char* msg;
			glfwGetError(&msg);
			//std::printf("Failed to create GLFW window - %s", msg);
		}
		glfwSetWindowUserPointer(glfw_wnd, this);

		glfwSetKeyCallback(glfw_wnd, key_callback);
		glfwSetCursorPosCallback(glfw_wnd, cursor_position_callback);
		glfwSetMouseButtonCallback(glfw_wnd, mouse_button_callback);
		glfwSetScrollCallback(glfw_wnd, scroll_callback);
		glfwSetWindowSizeCallback(glfw_wnd, window_resize_callback);
		glfwSetKeyCallback(glfw_wnd, key_callback);
	}

	OSWindowImpl(OSWindowImpl&& other) noexcept
	{
		std::swap(other.glfw_wnd, glfw_wnd);
	}
	OSWindowImpl& operator=(OSWindowImpl&& other) & noexcept
	{
		if (&other != this)
			std::swap(other.glfw_wnd, glfw_wnd);
		return *this;
	}
	~OSWindowImpl()
	{
		glfwSetWindowShouldClose(glfw_wnd, true);
		glfwDestroyWindow(glfw_wnd);
	}

	inline bool IsClosing() const noexcept
	{
		return glfwWindowShouldClose(glfw_wnd);
	}

private:
	OSWindowImpl(const OSWindowImpl&) = delete;
	OSWindowImpl& operator=(const OSWindowImpl&) = delete;

private:
	GLFWwindow* glfw_wnd = nullptr;
};

#pragma warning( disable : 4251)
struct API OSWindow final
{
	OSWindow(int width, int height, const char* title)
		: impl(width, height, title)
	{}

	template<typename ...Args>
	inline OSWindow& NewChild(Args && ...args)
	{
		OSWindow& wnd = children.emplace_back(std::forward<Args>(args)...);
		wnd.parent = this;
		wnd.child_it = std::prev(children.end());
		return wnd;
	}

	inline void RemoveFromParent() noexcept
	{
		if (parent)
			parent->children.erase(child_it);
	}

	inline bool IsClosing() const noexcept { return impl.IsClosing(); }
private:
	using ChildrenContainer = std::list<OSWindow>;
	using ContIter = typename ChildrenContainer::iterator;
private:
	OSWindowImpl impl;
	ChildrenContainer children;
	OSWindow* parent = nullptr;
	ContIter child_it;
};

/// Pointer on main_window
static std::unique_ptr<OSWindow> main_window = nullptr;

/// ========================= public API ========================

/// creates main window
WindowHandle API inCreateMainWindow(int width, int height, const char* title)
{
	assert(main_window == nullptr);
	glfwInit();
	main_window = std::make_unique<OSWindow>(width, height, title);
	return main_window.get();
}

/// creates child window
WindowHandle API inCreateChildWindow(WindowHandle parent, int width, int height, const char* title)
{
	return &parent->NewChild(width, height, title);
}

/// returns main window handle
WindowHandle API inGetMainWindow()
{
	return main_window.get();
}

/// returns main window handle
bool API inIsMainWindow(WindowHandle handle)
{
	return handle == main_window.get();
}

/// check if window should close
bool API inIsShouldClose(WindowHandle handle)
{
	return handle->IsClosing();
}

/// close and destroy window
void API inCloseWindow(WindowHandle handle)
{
	if (inIsMainWindow(handle))
	{
		main_window.reset();
		glfwTerminate();
	}
	else
		handle->RemoveFromParent();
}

void API inPollEvents()
{
	glfwPollEvents();
}