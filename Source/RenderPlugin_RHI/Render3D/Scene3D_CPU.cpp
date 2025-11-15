#include "Scene3D_CPU.hpp"

#include <Render3D/Scene3D_GPU.hpp>

namespace RenderPlugin
{
Scene3D_CPU::Scene3D_CPU(Scene3D_GPU & scene)
  : m_boundScene(&scene)
{
}
Scene3D_CPU::~Scene3D_CPU()
{
  if (m_boundScene)
  { 
    m_boundScene->SetCamera(m_camera);
    m_boundScene->TrySetCubes(m_cubesHash, m_cubesToDraw);
    m_boundScene->Draw();
  }
}

void Scene3D_CPU::AddCube(const GameFramework::Cube & cube)
{
  m_cubesToDraw.push_back(cube);
  GameFramework::Utils::hash_combine(m_cubesHash, cube);
}

void Scene3D_CPU::SetCamera(const GameFramework::Camera & camera)
{
  m_camera = camera;
}

} // namespace RenderPlugin
