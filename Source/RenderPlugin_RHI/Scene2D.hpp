#pragma once

#include <PluginInterfaces/RenderPlugin.hpp>

namespace RenderPlugin
{

struct Scene2D : public GameFramework::IRenderableScene2D
{
  explicit Scene2D(GameFramework::IDevice & device): device(device){}
  virtual ~Scene2D() override{}

  virtual void AddBackground(const std::array<float, 4> & color) override {}
  virtual void AddRect(float left, float top, float right, float bottom) override {}

private:
  GameFramework::IDevice & device;
};

} // namespace RenderPlugin
