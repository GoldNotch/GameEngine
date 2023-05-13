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
#include <cstdlib>
#include <memory>
#include <vector>
#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include "Renderer.hpp"

class OSWindow final
{
public:
/// ===================== Types =====================

/// =================== API ========================
    /// @brief move constructor
    OSWindow(OSWindow &&other) noexcept;
    /// @brief move operator
    OSWindow &operator=(OSWindow &&other) noexcept;
    /// destructor
    ~OSWindow() noexcept;

    //void SetRenderer(IRenderer& renderer) noexcept;
    void Close() noexcept;
    /// check is window should close
    bool IsClosing() const noexcept;
    /// @brief check if this window is main window of app
    inline bool IsMainWindow() const noexcept { return this == OSWindow::main_window.get(); }
    /// @brief creates and open new window
    OSWindow &CreateChildWindow(int width, int height, const char *title);
    template<typename T>
    T& InitRenderer() noexcept
    {  renderer = std::make_unique<T>();
        return dynamic_cast<T&>(*renderer);}
    /// create main window
    static OSWindow &CreateMainWindow(int width, int height, const char *title);
    /// Get main window
    static inline OSWindow &GetMainWindow() noexcept { return *OSWindow::main_window; }
    /// run all window loops. Block current thread
    static void RunWindows() noexcept;

private:
/// ================== Private Types =======================
    /// @brief  allocator to vector can construct with private constructor
    struct WindowAllocator : std::allocator<OSWindow>
    {
        template <class U, class... Args>
        void construct(U *p, Args &&...args)
        {
            ::new ((void *)p) U(std::forward<Args>(args)...);
        }

        template <class U>
        struct rebind
        {
            typedef WindowAllocator other;
        };
    };
    friend class WindowAllocator;

    struct ImplData;
/// =================== Data =============================

    static std::unique_ptr<OSWindow> main_window;           ///< global pointer on main window of app

    std::vector<OSWindow, WindowAllocator> child_windows;   ///< window contains its own sub windows
    std::unique_ptr<ImplData> impl_data;    ///< pointer on private data. Right now it's pointer on GLFWwindow structure
    std::unique_ptr<std::thread> rendering_thread = nullptr;///< child windows renders in separate thread. It's null in MainWindow
    std::unique_ptr<IRenderer> renderer = nullptr;
    double rendering_interval = 0.0;        ///< time interval between rendering in seconds (= Hz of monitor)
    double last_rendering_timestamp = 0.0;  ///< main window uses to skip rendering_interval time (main window used only)
    //IRenderer* renderer = nullptr;    
    std::mutex rendering_mutex;           
    double fps = 0.0;                       ///< framerate per second

/// ================== Service functions ================

    /// bind context before rendering
    void CreateContext() noexcept;
    /// One pass opf rendering
    void RenderPass(bool is_main_window) noexcept;
    /// Service function to run rendering threads for child windows
    void RunChildWindowsLoops(bool is_main_window) noexcept;
    /// init viewport
    void InitViewport() noexcept;
    /// Private constructor
    OSWindow(int width, int height, const char *title);
    OSWindow(const OSWindow &) = delete;
    OSWindow &operator=(const OSWindow &) = delete;
};