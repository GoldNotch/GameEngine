#include "Window.hpp"

#include <memory>
#include <stdexcept>

namespace GameFramework
{

Window_SDL::Window_SDL(const char * title, int width, int height)
{
  // create a window
  m_window = SDL_CreateWindow(title, width, height, SDL_WINDOW_RESIZABLE);
  if (!m_window)
    throw std::runtime_error("Failed to create window");

  m_drawTool = std::make_unique<DrawTool_SDL>(m_window);
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
    m_boundGame->GetGame().Render(*m_drawTool);
    m_drawTool->Flush();
  }
}

void Window_SDL::BindGameInstance(GameInstance * instance)
{
  m_boundGame = instance;
}

} // namespace GameFramework
