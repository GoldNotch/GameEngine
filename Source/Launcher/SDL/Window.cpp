#include "Window.hpp"

#include <memory>
#include <stdexcept>

#include <SDL/Rendering/ConnectionGPU.hpp>

namespace GameFramework
{

Window_SDL::Window_SDL(const char * title, int width, int height)
{
  // create a window
  m_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
  if (!m_window)
    throw std::runtime_error("Failed to create window");

  m_drawTool = std::make_unique<DrawTool_SDL>(GpuConnection(), m_window);
}

Window_SDL::~Window_SDL()
{
  m_drawTool.reset();
  // destroy the window
  SDL_DestroyWindow(m_window);
}

void Window_SDL::RenderFrame()
{
  if (m_boundGame)
  {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    m_boundGame->GetGame().Render({w, h}, *m_drawTool);
    m_drawTool->Finish();
  }
}

void Window_SDL::BindGameInstance(GameInstance * instance)
{
  m_boundGame = instance;
}

} // namespace GameFramework
