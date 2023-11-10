#pragma once
#include <vector>
#include <array>
#include <memory>
#include <functional>

#include <TypeMapping.hpp>
#include <Storage.hpp>

namespace world
{
	using scene3d = Core::HeterogeneousStorage</*enumerate all types which can be placed in scene*/void>;
}
