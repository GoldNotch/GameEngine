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
#include <cstdio>
#include "Window.hpp"
#include "App.hpp"
#include <thread>
#include <cmath>

extern "C"
{
    #include <GL/glew.h>
}

class TriangleApp : public IApp
{
    constexpr static float vertices[] = {-0.5f, -0.5f,
                                         0.5f, -0.5f,
                                         0.0f, 0.5f};
    /// called 60 times per second before rendering - collect data about visualizable scene
    virtual void BuildScene(RenderableScene &scene) const override
    {
        auto& triangle = scene.PlaceObject<StaticMeshObject>(StaticMeshObject::Geometry{vertices, 3});
    }

protected:
    virtual int AppMain() noexcept override
    {
        
        while (IsRunning()) {}
        return 0;
    }
};

struct MainRenderer : public IRenderer
{
    MainRenderer()
    {
        StaticMeshObject::InitForContext(*this);
    }

    void BindApp(const IApp* app)
    {this->app = app;}

    virtual void RenderPass(double timestamp) override
    {
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        if (app)
        {
            RenderableScene frame_data;
            app->BuildScene(frame_data);// long operation
            PrepareRenderData(frame_data);
            BindGroup(STATIC_MESH_OBJECT);
            /*for(auto& renderable: frame_data.objects)
                renderable->Render(*this);*/
            // render mesh objects
        }
    }
    
protected:
    virtual void PrepareRenderData(const RenderableScene& scene) override
    {
        //traverse objects in scene and check vbos
    }
private:
    const IApp* app = nullptr;
};

int main()
{
    // Renderer renderer;
    OSWindow &main_wnd = OSWindow::CreateMainWindow(500, 600, "main_window");
    OSWindow &sub_wnd = main_wnd.CreateChildWindow(400, 400, "child window");
    auto& renderer = main_wnd.InitRenderer<MainRenderer>();
    TriangleApp app;
    app.SetOnFinishCallback([&](int result_code)
                            {
        main_wnd.Close();
        std::printf("application finished with %i\n", result_code); });
    renderer.BindApp(&app);

    app.Run();
    OSWindow::RunWindows();
    app.Close();
    return 0;
}