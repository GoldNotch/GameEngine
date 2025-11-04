set(this_target "RenderPlugin_RHI")

add_library(${this_target} SHARED)

target_sources(${this_target}
PRIVATE
	"dllmain.cpp"
	"Constants.hpp"
	"ScreenDevice.cpp"
	"ScreenDevice.hpp"
	"Scene2D.cpp" 
	"Scene2D.hpp" 
	"Scene2DProxy.cpp"
	"Scene2DProxy.hpp"
	
	"ShaderFile.hpp"
	"ShaderFile.cpp"

	"Renderer/BackgroundRenderer.cpp"
	"Renderer/BackgroundRenderer.hpp"
)

target_include_directories(${this_target} 
PUBLIC
	"${SOURCE_DIR}/RenderPlugin_RHI"
)

target_link_libraries(${this_target}
PRIVATE
	GameFramework
	RHI
)

target_compile_definitions(${this_target}
PRIVATE
	SHADERS_DIRECTORY=".shaders"
)

include(CompileSpirV.cmake)

TARGET_PRECOMPILE_SPIRV(
TARGET ${this_target} 
SHADERS_DIRECTORY ".shaders"
SHADERS
	"Shaders/background.vert"
	"Shaders/background.frag"
	"Shaders/rect2d.frag"
	"Shaders/rect2d.vert"
)