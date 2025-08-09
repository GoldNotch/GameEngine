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
struct DrawTool_SDL : public IDrawTool
{
  explicit DrawTool_SDL(SDL_Window * wnd);
  virtual ~DrawTool_SDL() override;

  void Finish();
  SDL_GPUDevice * GetDevice() const noexcept;
  Uploader & GetUploader() & noexcept;

  SDL_GPUShader * BuildSpirVShader(const std::filesystem::path & path, SDL_GPUShaderStage stage,
                                   uint32_t numSamplers = 0, uint32_t numUniforms = 0,
                                   uint32_t numSSBO = 0, uint32_t numSSTO = 0) const;

  //------------- IDrawTool Interface -------------
  virtual void Flush() override;
  virtual void SetClearColor(const std::array<float, 4> & color) override;
  virtual void DrawRect(float left, float top, float right, float bottom) override;
  virtual void DrawMesh(IResource * mesh) override;

private:
  SDL_GPUDevice * m_gpu = nullptr;
  SDL_Window * m_window = nullptr; ///< doesn't own

  std::array<float, 4> m_clearColor;

  std::unique_ptr<QuadRenderer> m_quadRenderer;
  std::unique_ptr<MeshRenderer> m_meshRenderer;
  std::unique_ptr<Uploader> m_uploader;
};
} // namespace GameFramework
