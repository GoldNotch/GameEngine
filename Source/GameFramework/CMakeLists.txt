set(this_target "GameFramework")

add_library(${this_target} SHARED)

target_sources(${this_target}
PUBLIC
	"GameFramework.hpp"
	"PluginInterfaces/GamePlugin.hpp"
	"PluginInterfaces/DrawToolPlugin.hpp"
	"Plugin/Plugin.hpp"
	"Game/Signal.hpp"
	"Game/Input.hpp"
PRIVATE
	"GameFramework_def.h"
	"GameFramework.cpp"
	"Core/Formatter.hpp"
	"Core/Storage.hpp"
	"Core/TypeMapping.hpp"
	"Core/StringUtils.hpp"
	"Plugin/Plugin.cpp"
	"Game/Signal.cpp" 
	"Game/InputBinding.hpp" 
	"Game/InputBinding.cpp" 
	"Game/InputController.hpp"
	"Game/InputController.cpp" 
	"Game/InputQueue.cpp"
	"Game/InputQueue.hpp"
 "Game/Time.cpp")

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${this_target}
PRIVATE
	-DGAME_FRAMEWORK_BUILD
)

target_include_directories(${this_target}
	PUBLIC
		"${SOURCE_DIR}/GameFramework"
)
add_subdirectory(Tests)

target_link_libraries(${this_target}
PRIVATE
	dylib
	concurrentqueue
)