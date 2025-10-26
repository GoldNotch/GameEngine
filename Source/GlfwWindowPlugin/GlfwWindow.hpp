#pragma once

#include <PluginInterfaces/WindowPlugin.hpp>

namespace GlfwPlugin
{
GameFramework::WindowUPtr NewWindowImpl(const std::string & title, int width, int height);
}
