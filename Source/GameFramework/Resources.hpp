#pragma once
#include <GameFramework_def.h>

#include <chrono>
#include <filesystem>

#include <Resources/StaticMesh.hpp>

namespace GameFramework
{

GAME_FRAMEWORK_API IStaticMeshResource * LoadStaticMesh(const char * path);

} // namespace GameFramework
