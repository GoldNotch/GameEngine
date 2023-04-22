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
#include <vector>
#include <thread>
#include <memory>
#include <functional>
struct RenderSceneData;

struct IApp
{
    using FinishCallback = std::function<void(int result_code)>;
    IApp() = default;
    /// @brief destructor
    virtual ~IApp() { Close(); }
    /// @brief set callback for finish event
    /// @param on_finish_func - callback function
    void SetOnFinishCallback(FinishCallback on_finish_func) { on_finish = on_finish_func; }
    /// @brief launch app-thread
    void Run() noexcept
    {
        is_running = true;
        app_thread.reset(new std::thread([this] {
            int result = AppMain();
            if (on_finish) on_finish(result);
            is_running = false;
        }));
    }
    /// @brief close app. Join app-thread
    void Close() noexcept
    {
        is_running = false;
        if (app_thread)
            app_thread->join();
        app_thread.reset();
    }
    /// @brief check if app is running
    /// @return true if app-thread is running
    bool IsRunning() const noexcept { return is_running; }

    virtual void BuildScene(RenderSceneData &scene) const = 0;

protected:
    /// @brief execution for app
    /// @return error code for app
    virtual int AppMain() noexcept = 0;

private:
    std::unique_ptr<std::thread> app_thread = nullptr;
    FinishCallback on_finish = nullptr;
    std::atomic_bool is_running = false;
    IApp(const IApp &) = delete;
    IApp &operator=(IApp &) = delete;
};


