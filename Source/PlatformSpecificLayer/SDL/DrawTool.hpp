#pragma once
#include <GameFramework.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct DrawTool_SDL : public IDrawTool
{
  explicit DrawTool_SDL(SDL_Window * wnd);
  virtual ~DrawTool_SDL() override;

  void Flush();

private:
  SDL_GPUDevice * m_gpu = nullptr;
  SDL_Window * m_window = nullptr; ///< doesn't own
};
} // namespace GameFramework
