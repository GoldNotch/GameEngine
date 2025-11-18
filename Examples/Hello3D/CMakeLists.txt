set(this_target "Hello3D")

add_library(${this_target} SHARED
	"dllmain.cpp"
	"FPSCamera.cpp"
	"FPSCamera.hpp"
)

target_link_libraries(${this_target}
PUBLIC
	GameFramework
	glm
)