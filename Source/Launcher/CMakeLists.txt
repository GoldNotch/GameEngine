set(this_target "Launcher")
include (ShaderCompile.cmake)

add_executable(${this_target})

target_sources(${this_target}
PUBLIC
	"Launcher.hpp"
PRIVATE
	"Launcher_def.h"
	"Launcher.cpp"
	"OwnedBy.hpp"
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
	"SDL/Rendering/MeshRenderer.cpp"
	"SDL/Rendering/MeshRenderer.hpp"
	"SDL/Rendering/Uploader.cpp"
	"SDL/Rendering/Uploader.hpp"
 "SDL/Rendering/MeshRenderer.cpp")

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

TARGET_PRECOMPILE_SHADERS(
	TARGET ${this_target}
	SHADERS_DIRECTORY "SDL/Rendering/Shaders"
	SHADERS
		"quad.vert"
		"quad.frag"
		"mesh.vert"
		"mesh.frag"
)
