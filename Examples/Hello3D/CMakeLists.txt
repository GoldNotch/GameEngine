set(this_target "Hello3D")

add_library(${this_target} SHARED
	"dllmain.cpp"
	"FPSCamera.cpp"
	"FPSCamera.hpp"
)

find_package(glm REQUIRED)

target_link_libraries(${this_target}
PUBLIC
	GameFramework
	glm::glm
)

TARGET_COPY_ASSETS(
	TARGET ${this_target}
	ASSETS_DIRECTORY "Data"
	ASSETS
	"assets.csv"
	"Shaders/Cube.frag"
)