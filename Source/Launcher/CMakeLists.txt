set(this_target "Launcher")
set(ImplFolder "SDL")

add_executable(${this_target})

target_sources(${this_target}
PUBLIC
	"Launcher.hpp"
PRIVATE
	"Launcher_def.h"
	"Launcher.cpp"
	"Games/GameLibrary.cpp"
	"Games/GameLibrary.hpp"
	"Games/GameInstance.cpp"
	"Games/GameInstance.hpp"
	"PlatformSpecificLayer/${ImplFolder}/Application.cpp"
	"PlatformSpecificLayer/${ImplFolder}/Window.cpp"
	"PlatformSpecificLayer/${ImplFolder}/Window.hpp"
	"PlatformSpecificLayer/${ImplFolder}/DrawTool.cpp"
	"PlatformSpecificLayer/${ImplFolder}/DrawTool.hpp"
)

target_link_libraries(${this_target}
PRIVATE
	SDL3-static
	dylib
PRIVATE
	GameFramework
)

target_compile_definitions(${this_target} 
	PRIVATE
		-DLAUNCHER_BUILD
)


target_include_directories(${this_target} 
PUBLIC
	"${SOURCE_DIR}/Launcher"
)
