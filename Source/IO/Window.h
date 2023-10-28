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

#ifndef WINDOW_H
#define WINDOW_H

#if defined (BUILD_STATIC) || !defined(_WIN32)
#define API
#else
#ifdef BUILD_DLL
#define API __declspec(dllexport)
#else
#define API __declspec(dllimport)
#endif
#endif

#ifdef __cplusplus 
extern "C" {
#endif


	struct API OSWindow;
	typedef struct OSWindow* WindowHandle;


	/// creates main window
	WindowHandle API inCreateMainWindow(int width, int height, const char* title);
	/// creates child window
	WindowHandle API inCreateChildWindow(WindowHandle parent, int width, int height, const char* title);
	/// returns main window handle
	WindowHandle API inGetMainWindow();
	/// check if window is main
	bool API inIsMainWindow(WindowHandle handle);
	/// check if window should close
	bool API inIsShouldClose(WindowHandle handle);
	/// close and destroy window
	void API inCloseWindow(WindowHandle handle);

	void API inPollEvents();

	enum EKeys
	{

	};

#ifdef __cplusplus
}
#endif

/*class OSWindow final
{
	//TODO: make event-oriented mode for rendering calls (i.e. call rendering as events on user actions)
	//TODO: extract renderer into separate class, because of OSWindow has many responsibilities
public:
	/// ===================== Types =====================

	/// =================== API ========================
	/// @brief move constructor
	OSWindow(OSWindow&& other) = default;
	/// @brief move operator
	OSWindow& operator=(OSWindow&& other) = default;
	/// destructor
	~OSWindow();

	/// @brief check if this window is main window of app
	inline bool IsMainWindow() const noexcept { return this == OSWindow::main_window.get(); }
	/// @brief creates and open new window
	OSWindow& CreateChildWindow(int width, int height, const char* title)&;

	/// create main window
	static OSWindow& CreateMainWindow(int width, int height, const char* title);
	/// Get main window
	static inline OSWindow& GetMainWindow() noexcept { return *OSWindow::main_window; }
	/// run all threads and processes
	static void RunWindows();

private:
	/// ================== Private Types =======================
		/// @brief  allocator to vector can construct with private constructor
	struct WindowAllocator : std::allocator<OSWindow>
	{
		template <class U, class... Args>
		void construct(U* p, Args &&...args)
		{
			::new (reinterpret_cast<void*>(p)) U(std::forward<Args>(args)...);
		}

		template <class U>
		struct rebind{ typedef WindowAllocator other; };
	};
	friend struct WindowAllocator;

	class ImplData;
	/// =================== Data =============================
private:
	static std::unique_ptr<OSWindow> main_window;           ///< global pointer on main window of app
	static std::unique_ptr<std::thread> input_thread;		///< thread for input handling
	std::vector<OSWindow, WindowAllocator> child_windows;   ///< window contains its own sub windows

	std::unique_ptr<ImplData> impl_data;    ///< pointer on private data. Right now it's pointer on GLFWwindow structure

/// ================== Service functions ================
private:
	/// check is window should close
	bool IsClosing() const noexcept;
	static void EventsHandlingMain();

	/// Private constructor
	OSWindow(int width, int height, const char* title);
	OSWindow(const OSWindow&) = delete;
	OSWindow& operator=(const OSWindow&) = delete;
};*/

#endif //WINDOW_H