set(this_target "Launcher")

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
	
	"SDL/Window.cpp"
	"SDL/Window.hpp"
	"SDL/Rendering/DrawTool.cpp"
	"SDL/Rendering/DrawTool.hpp"
	"SDL/Rendering/QuadRenderer.cpp"
	"SDL/Rendering/QuadRenderer.hpp"
)

target_link_libraries(${this_target}
PRIVATE
	SDL3::SDL3
	SDL3_image::SDL3_image
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
