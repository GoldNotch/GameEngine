#include "App.hpp"

#include <Window.h>
#include "Scene3D.hpp"

namespace Engine
{

	void RunApp(IApp& app)
	{
		// create window
		WindowHandle wnd = inCreateMainWindow(800, 600, "Test Title");

		Core::HeterogeneousStorage<int, double, float> store;
		store.emplace<int>(5);
		store.emplace<float>(25.0f);
		store.emplace<double>(10.0);
		store.emplace<char>('t'); // compile-time error

		/*while (!inIsShouldClose(wnd))
		{
			inPollEvents();
			 update scene

			 render

		}*/
		inCloseWindow(wnd);

		// main loop
		//while (true)
		//{
			// PROCESS ALL SUBSYSTEMS (Rendering, Audio, GameLogic, Phisics, etc)
			// But every subsystem is updated with various rate
			// (f.e. rendering is 60Hz, phisics - 120Hz, gamelogic - 1 Hz, etc)
			// Also some subsystems depends from others.
			// I guess it can be solved by graph of tasks.
			// You create a graph where nodes are tasks, when we traverse this graph and layout in linear queue

			// process user input

			// game update 
				// phisics
				// AI 
				// Scripts

			// rendering (audio + video)
		//}
	}

}