#include "Scene2DProxy.hpp"

#include <Scene2D.hpp>

namespace RenderPlugin
{

Scene2DProxy::Scene2DProxy(Scene2D & scene)
  : m_scene(&scene)
{
}

Scene2DProxy::~Scene2DProxy()
{
  assert(m_scene);
  m_scene->Draw();
}

void Scene2DProxy::AddBackground(const std::array<float, 4> & color)
{
  assert(m_scene);
  m_scene->AddBackground(color);
}

void Scene2DProxy::AddRect(float left, float top, float right, float bottom)
{
  assert(m_scene);
  m_scene->AddRect(left, top, right, bottom);
}
} // namespace RenderPlugin
