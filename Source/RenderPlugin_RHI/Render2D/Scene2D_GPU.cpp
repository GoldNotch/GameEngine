#include "Scene2D_GPU.hpp"

#include <Constants.hpp>
#include <Files/FileManager.hpp>
#include <ShaderFile.hpp>

namespace RenderPlugin
{

Scene2D_GPU::Scene2D_GPU(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer)
  : m_backgroundRenderer(ctx, framebuffer)
  , m_rectsRenderer(ctx, framebuffer)
{
}

Scene2D_GPU::~Scene2D_GPU() = default;

void Scene2D_GPU::SetBackground(const GameFramework::Color3f & color)
{
  m_backgroundRenderer.SetBackground(color);
}

void Scene2D_GPU::TrySetRects(size_t newHash, std::span<const GameFramework::Rect2d> rects)
{
  m_rectsRenderer.TrySetRects(newHash, rects);
}

void Scene2D_GPU::Invalidate()
{
  //TODO: m_renderPass->SetDirtyCommands();
}

void Scene2D_GPU::Draw()
{
  m_backgroundRenderer.Submit();
  m_rectsRenderer.Submit();
}

bool Scene2D_GPU::ShouldBeInvalidated() const noexcept
{
  return false;
}

} // namespace RenderPlugin
