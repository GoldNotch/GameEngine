#pragma once
#include "../Engine.h"
#include "../Core/Storage.hpp"

using RenderableScene = Core::Storage<usMeshObjectConstructorArgs>;

/// @brief create and init rendering system
/// @param opts
void InitRenderingSystem(const usRenderingOptions & opts);

/// @brief destroy all objects and clear all resources in rendering system
void TerminateRenderingSystem();

/// @brief render scene 
/// @param scene 
void Render2D(const RenderableScene & scene);
