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

#include "Scene2D.hpp"
using namespace Framework;
// ---------------------------- client app ----------------------------

class TriangleApp : public IApp
{
	constexpr static float vertices[] = { -0.5f, -0.5f,
										 0.5f, -0.5f,
										 0.0f, 0.5f };
	/// called 60 times per second before rendering - collect data about visualizable scene
	std::unique_ptr<IRenderableScene> BuildScene() const override
	{
		std::unique_ptr<scene2d::Scene> scene = std::make_unique<scene2d::Scene>();
		const auto& triangle_mesh = scene->NewGeometry<scene2d::StaticMeshBuilder::Vertex>(
			CPUBufferPtr<float>(vertices, 6).ReinterpretCast<scene2d::StaticMeshBuilder::Vertex>()
			);
		scene2d::SceneObject obj(scene2d::ObjectTypes::STATIC_MESH, scene2d::Placement{}, &triangle_mesh);
		scene->PlaceObject(obj);
		return scene;
	}

protected:
	int AppMain() noexcept override
	{
		// here is app logic
		while (IsRunning())
		{
			BeginFrame();
			// todo some logic
			EndFrame();
		}
		return 0;
	}
};

// ---------------------------- main -------------------------------

int main()
{
	// Renderer renderer;
	OSWindow& main_wnd = OSWindow::CreateMainWindow(500, 600, "main_window");
	auto& renderer = main_wnd.InitRenderer<scene2d::Renderer>();
	// init window and renderer
	// ...

	// create and run app
	TriangleApp app {main_wnd};
	app.SetOnFinishCallback([&](int result_code)
		{
			std::printf("application finished with %i\n", result_code);
		});
	app.Run();
	return 0;
}