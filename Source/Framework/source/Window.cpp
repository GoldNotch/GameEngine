///Multiplatform implementation of Window.hpp
#include "Window.hpp"


namespace Framework
{
	std::unique_ptr<OSWindow> OSWindow::main_window = nullptr;
	std::unique_ptr<std::thread> OSWindow::input_thread = nullptr;

	void OSWindow::RunWindows()
	{
		OSWindow::input_thread.reset(new std::thread(OSWindow::EventsHandlingMain));
	}
}