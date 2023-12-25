#pragma once
#include "../Engine.h"

/// @brief create and init rendering system
/// @param opts
void InitRenderingSystem(const usRenderingOptions & opts);

/// @brief destroy all objects and clear all resources in rendering system
void TerminateRenderingSystem();
