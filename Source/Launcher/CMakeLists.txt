set(this_target "Launcher")
include (ShaderCompile.cmake)

add_executable(${this_target})

target_sources(${this_target}
PRIVATE
	"main.cpp"
	"GlfwInstance.hpp"
	"GlfwInstance.cpp"
	"IWindow.hpp"
	"GlfwWindow.cpp"
)

target_link_libraries(${this_target}
PRIVATE
	GameFramework
	GameSystems
)

target_compile_definitions(${this_target} 
	PRIVATE
		-DLAUNCHER_BUILD
)


target_include_directories(${this_target} 
PUBLIC
	"${SOURCE_DIR}/Launcher"
)

target_link_libraries(${this_target}
PRIVATE
	glfw
)