#pragma once

#include <GameFramework.hpp>
#include <InternalDeviceInterface.hpp>
#include <Render3D/Renderer/CubeRenderer.hpp>
#include <RHI.hpp>

namespace RenderPlugin
{

struct ViewProjection final
{
  GameFramework::Mat4f view;
  GameFramework::Mat4f projection;
};

struct Scene3D_GPU final : public RHI::OwnedBy<InternalDevice>
{
  explicit Scene3D_GPU(InternalDevice & device);
  virtual ~Scene3D_GPU() override;
  MAKE_ALIAS_FOR_GET_OWNER(InternalDevice, GetDevice);

  void TrySetCubes(size_t newHash, std::span<const GameFramework::Cube> cubes);
  void SetCamera(const GameFramework::Camera & camera);

public:
  RHI::IBufferGPU * GetViewProjectionBuffer();

public:
  void Invalidate();
  void Draw();
  bool ShouldBeInvalidated() const noexcept;

private:
  RHI::IBufferGPU * m_viewProjBuffer = nullptr;
  CubeRenderer m_cubesRenderer; // one for each material
};

} // namespace RenderPlugin

/*
Каждый рендерер привязан к одному материалу
При присвоении кубиков должны распределить все кубики по рендерерам в зависимости от материала
палитра материалов храниться здесь в Scene3D_GPU
материал настривает пайплайн - загружает и привязывает фрагментный шейдер, выделяет буфер сэмплеров

*/
