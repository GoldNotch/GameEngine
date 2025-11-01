set(this_target "GameFramework")

add_library(${this_target} SHARED)

target_sources(${this_target}
PUBLIC
	"GameFramework.hpp"
	"PluginInterfaces/GamePlugin.hpp"
	"PluginInterfaces/RenderPlugin.hpp"
	"PluginInterfaces/WindowsPlugin.hpp"
PRIVATE
	"GameFramework_def.h"
	"GameFramework.cpp"

	"Utility/Formatter.hpp"
	"Utility/Storage.hpp"
	"Utility/TypeMapping.hpp"
	"Utility/StringUtils.hpp"

	"Plugin/Plugin.cpp"
	"Plugin/Plugin.hpp"

	"Game/Signal.cpp" 
	"Game/Signal.hpp"
	"Game/Time.cpp"
	"Game/Time.hpp"

	"Files/FileStream.hpp"
	"Files/MountPoint.hpp"
	"Files/FileManager.hpp"
	"Files/FileManager.cpp"
	 "Files/StandardFileStream.cpp"

	"Input/Input.hpp"
	"Input/InputDevice.hpp"
	"Input/InputBinding.hpp" 
	"Input/InputBinding.cpp" 
	"Input/InputController.hpp"
	"Input/InputController.cpp" 
	"Input/InputQueue.cpp"
	"Input/InputQueue.hpp"
)

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