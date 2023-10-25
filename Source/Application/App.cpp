#include "App.hpp"

#include <Window.h>

namespace Engine
{

	void RunApp(IApp& app)
	{
		// create window
		WindowHandle wnd = inCreateMainWindow(800, 600, "Test Title");

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