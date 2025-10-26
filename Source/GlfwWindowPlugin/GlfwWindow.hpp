#pragma once

#include <PluginInterfaces/WindowsPlugin.hpp>

namespace GlfwWindowsPlugin
{
GameFramework::WindowUPtr NewWindowImpl(const std::string & title, int width, int height);
}
