#pragma once

#include <unordered_map>

#include <GameFramework.hpp>
#include <Render3D/RenderData.hpp>
#include <Render3D/Renderer/CubeRenderer.hpp>
#include <RHI.hpp>
#include <Utility/InternalDeviceInterface.hpp>

namespace RenderPlugin
{

template<typename RendererT>
using Renderers =
  std::unordered_map<std::filesystem::path /*shaderPath*/, std::unique_ptr<RendererT>>;

struct Scene3D_GPU final : public RHI::OwnedBy<InternalDevice>
{
  explicit Scene3D_GPU(InternalDevice & device);
  virtual ~Scene3D_GPU() override;
  MAKE_ALIAS_FOR_GET_OWNER(InternalDevice, GetDevice);

  void TrySetCubes(RenderableBatches<GameFramework::Cube> && batches);
  void SetCamera(const GameFramework::Camera & camera);

public:
  RHI::IBufferGPU * GetViewProjectionBuffer();

public:
  void Invalidate();
  void Draw();
  bool ShouldBeInvalidated() const noexcept;

private:
  RHI::IBufferGPU * m_viewProjBuffer = nullptr;
  Renderers<CubeRenderer> m_cubeRenderers;
};

} // namespace RenderPlugin

/*
 аждый рендерер прив€зан к одному материалу
ѕри присвоении кубиков должны распределить все кубики по рендерерам в зависимости от материала
палитра материалов хранитьс€ здесь в Scene3D_GPU
материал настривает пайплайн - загружает и прив€зывает фрагментный шейдер, выдел€ет буфер сэмплеров
кто владеет текстурами? - контекст
должен быть асинхронный загрузчик текстур, чтобы сама загрузка производилась только в пределах потока, где выполн€етс€ TransferPass
≈сли текстура уже загружена, то не загружаем второй раз
может произойти переполнение размера под текстуры, тогда загрузчик должен выгружать неиспользуемые текстуры


*/
