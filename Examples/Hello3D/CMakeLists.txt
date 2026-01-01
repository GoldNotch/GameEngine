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

TARGET_COPY_ASSETS(
	TARGET ${this_target}
	ASSETS_DIRECTORY "Data"
	ASSETS
	"assets.csv"
	"Shaders/Cube.frag"
)