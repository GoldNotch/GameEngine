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
    m_boundScene->TrySetCubes(std::move(m_cubeBatches));
    m_boundScene->Draw();
  }
}

void Scene3D_CPU::AddCube(const GameFramework::Cube & cube)
{
  std::filesystem::path shaderPath = cube.GetMaterial() ? cube.GetMaterial()->GetMaterialShader()
                                                        : L"";
  auto [it, inserted] = m_cubeBatches.insert({shaderPath, {}});
  GameFramework::Utils::hash_combine(it->second.first, cube);
  it->second.second.push_back(cube);
}

void Scene3D_CPU::SetCamera(const GameFramework::Camera & camera)
{
  m_camera = camera;
}

} // namespace RenderPlugin
