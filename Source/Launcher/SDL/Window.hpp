#pragma once

#include <Games/GameInstance.hpp>
#include <SDL3/SDL.h>

#include "SDL/Rendering/DrawTool.hpp"

namespace GameFramework
{

class Window_SDL
{
  SDL_Window * m_window = nullptr;
  std::unique_ptr<DrawTool_SDL> m_drawTool;
  GameInstance * m_boundGame = nullptr; ///< doesn't own

public:
  explicit Window_SDL(const char * title, int width, int height);
  ~Window_SDL();

  void RenderFrame();
  void BindGameInstance(GameInstance * instance);
};
} // namespace GameFramework
