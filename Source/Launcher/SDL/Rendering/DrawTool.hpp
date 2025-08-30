#pragma once
#include <array>
#include <filesystem>

#include <GameFramework.hpp>
#include <SDL3/SDL.h>

#include "MeshRenderer.hpp"
#include "QuadRenderer.hpp"
#include "Uploader.hpp"

namespace GameFramework
{
struct ConnectionGPU;

struct DrawTool_SDL : public OwnedBy<ConnectionGPU>,
                      public IDrawTool
{
  explicit DrawTool_SDL(ConnectionGPU & gpu, SDL_Window * wnd);
  virtual ~DrawTool_SDL() override;
  MAKE_ALIAS_FOR_GET_OWNER(ConnectionGPU, GetGPU);

  void Finish();

  //------------- IDrawTool Interface -------------
  virtual void SetClearColor(const glm::vec4 & color) override;
  virtual void SetViewport(int x, int y, int width, int height) override {};
  virtual void SetClientRect(int width, int height) override {};
  virtual void DrawRect(float left, float top, float right, float bottom) override;
  virtual void DrawMesh(IStaticMeshResource * mesh) override;

private:
  SDL_Window * m_window = nullptr; ///< doesn't own

  glm::vec4 m_clearColor;

  std::unique_ptr<QuadRenderer> m_quadRenderer;
  std::unique_ptr<MeshRenderer> m_meshRenderer;
};
} // namespace GameFramework
