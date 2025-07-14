#pragma once
#include <array>

#include <GameFramework.hpp>
#include <SDL3/SDL.h>

namespace GameFramework
{
struct DrawTool_SDL : public IDrawTool
{
  explicit DrawTool_SDL(SDL_Window * wnd);
  virtual ~DrawTool_SDL() override;

  void Finish();

  //------------- IDrawTool Interface -------------
  virtual void Flush() override;
  virtual void SetClearColor(const std::array<float, 4> & color) override;


private:
  SDL_GPUDevice * m_gpu = nullptr;
  SDL_Window * m_window = nullptr; ///< doesn't own

  std::array<float, 4> m_clearColor;
};
} // namespace GameFramework
