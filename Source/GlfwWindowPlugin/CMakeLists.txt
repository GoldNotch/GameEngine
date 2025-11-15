set(this_target "GlfwWindowPlugin")

add_library(${this_target} SHARED)

target_sources(${this_target}
PRIVATE
	"dllmain.cpp"
	"GlfwInstance.hpp"
	"GlfwInstance.cpp"
	"GlfwWindow.cpp"
	"GlfwInput.cpp"
	"GlfwInput.hpp"
)

target_include_directories(${this_target} 
PUBLIC
	"${SOURCE_DIR}/GlfwWindowPlugin"
)

target_link_libraries(${this_target}
PRIVATE
	GameFramework
	glfw
)