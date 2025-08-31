#pragma once
#include <array>
#include <filesystem>

#include <GameFramework.hpp>
#include <SDL3/SDL.h>

#include "MeshRenderer.hpp"
#include "QuadRenderer.hpp"
#include "RenderTarget.hpp"
#include "Uploader.hpp"

namespace GameFramework
{
struct ConnectionGPU;

struct DrawTool_SDL : public OwnedBy<ConnectionGPU>,
                      public IDrawTool
{
  explicit DrawTool_SDL(ConnectionGPU & gpu);
  virtual ~DrawTool_SDL() override;
  MAKE_ALIAS_FOR_GET_OWNER(ConnectionGPU, GetGPU);

  void Finish();

  const RenderTarget & GetRenderTarget() const & noexcept { return m_renderTarget; }
  const glm::vec4 & GetClearColor() const & noexcept { return m_clearColor; }

  RenderTarget ExchangeRenderTarget(RenderTarget && rt) noexcept;

  //------------- IDrawTool Interface -------------
  virtual void SetClearColor(const glm::vec4 & color) override;
  virtual void SetViewport(int x, int y, int width, int height) override {};
  virtual void SetClientRect(int width, int height) override {};
  virtual void DrawRect(float left, float top, float right, float bottom) override;
  virtual void DrawMesh(IStaticMeshResource * mesh) override;

private:
  RenderTarget m_renderTarget;

  glm::vec4 m_clearColor;

  std::unique_ptr<QuadRenderer> m_quadRenderer;
  std::unique_ptr<MeshRenderer> m_meshRenderer;
};

struct WindowDC_SDL final : public OwnedBy<DrawTool_SDL>
{
  WindowDC_SDL(DrawTool_SDL & drawTool, SDL_Window * window);
  virtual ~WindowDC_SDL() override;
  MAKE_ALIAS_FOR_GET_OWNER(DrawTool_SDL, GetDrawTool);

  bool AcquireBuffer();
  void SubmitBuffer();

private:
  SDL_Window * m_window = nullptr; ///< doesn't own
  RenderTarget m_oldRt;
};
} // namespace GameFramework
