#include "Scene2D.hpp"

#include <Constants.hpp>
#include <Files/FileManager.hpp>
#include <ShaderFile.hpp>

namespace RenderPlugin
{

Scene2D::Scene2D(RHI::IContext & ctx, RHI::IFramebuffer & framebuffer)
  : m_backgroundRenderer(ctx, framebuffer)
{
}

Scene2D::~Scene2D() = default;

void Scene2D::AddBackground(const std::array<float, 4> & color)
{
  m_backgroundRenderer.SetBackground(color[0], color[1], color[2]);
}

void Scene2D::AddRect(float left, float top, float right, float bottom)
{
}

void Scene2D::Invalidate()
{
  //TODO: m_renderPass->SetDirtyCommands();
}

void Scene2D::Draw()
{
  m_backgroundRenderer.Submit();
}

bool Scene2D::ShouldBeInvalidated() const noexcept
{
    return false;
}

} // namespace RenderPlugin
