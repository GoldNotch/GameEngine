#include "App.hpp"

#include <Window.h>
#include "ObjectsContainer.hpp"

namespace Engine
{

	void RunApp(IApp& app)
	{
		// create window
		WindowHandle wnd = inCreateMainWindow(800, 600, "Test Title");

		Core::Storage<int> t;
		std::vector<int*> ints;
		for(size_t i = 0; i < 100; ++i)
			ints.push_back(&t.emplace(rand() % 100));

		for(auto && n : t)
			std::printf("%i ", n);
		std::printf("\n");
		for(auto it = ints.rbegin(); it != ints.rend(); ++it)
		{
			auto t_it = Core::Storage<int>::iterator(t, **it);
			int d = std::distance(t.begin(), t_it);
			std::printf("%i ", d);
			t.erase(t_it);
		}

		while (!inIsShouldClose(wnd))
		{
			inPollEvents();
			// update scene

			// render

		}
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