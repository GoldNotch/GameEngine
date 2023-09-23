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
#include "../Framework/App.hpp"
#include <thread>
#include <cmath>

#include "../Framework/Scene2D.hpp"
using namespace Framework;
// ---------------------------- client app ----------------------------

class TriangleApp : public IApp
{
	constexpr static float vertices[] = { -0.5f, -0.5f,
										 0.5f, -0.5f,
										 0.0f, 0.5f };
public:
	using IApp::IApp;
protected:
	int AppMain() noexcept override
	{
		// here is app logic
		while (IsRunning())
		{
			BeginTick();
			// todo some logic
			EndTick();
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
	TriangleApp app(std::move(main_wnd));
	app.SetOnFinishCallback([&](int result_code)
		{
			std::printf("application finished with %i\n", result_code);
		});
	app.Run();
	return 0;
}