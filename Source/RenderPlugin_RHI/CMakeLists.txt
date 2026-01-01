set(this_target "RenderPlugin_RHI")

add_library(${this_target} SHARED)

target_sources(${this_target}
PRIVATE
	"dllmain.cpp"
	"Constants.hpp"
	"InternalDeviceInterface.hpp"
	"ScreenDevice.cpp"
	"ScreenDevice.hpp"
	"ShaderFile.hpp"
	"ShaderFile.cpp"

	"Render2D/Scene2D_GPU.cpp" 
	"Render2D/Scene2D_GPU.hpp" 
	"Render2D/Scene2D_CPU.cpp"
	"Render2D/Scene2D_CPU.hpp"
	"Render2D/Renderer/BackgroundRenderer.cpp"
	"Render2D/Renderer/BackgroundRenderer.hpp"
	"Render2D/Renderer/Rect2dRenderer.cpp"
	"Render2D/Renderer/Rect2dRenderer.hpp"

	"Render3D/Scene3D_CPU.cpp"
	"Render3D/Scene3D_CPU.hpp"
	"Render3D/Scene3D_GPU.cpp"
	"Render3D/Scene3D_GPU.hpp"
	"Render3D/Renderer/CubeRenderer.cpp"
	"Render3D/Renderer/CubeRenderer.hpp"
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
	"Render2D/Shaders/background.vert"
	"Render2D/Shaders/background.frag"
	"Render2D/Shaders/rect2d.frag"
	"Render2D/Shaders/rect2d.vert"

	"Render3D/Shaders/Cube.frag"
	"Render3D/Shaders/Cube.vert"
)