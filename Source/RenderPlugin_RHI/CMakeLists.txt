set(this_target "RenderPlugin_RHI")

add_library(${this_target} SHARED)

target_sources(${this_target}
PRIVATE
	"dllmain.cpp"
	"ScreenDevice.cpp"
	"ScreenDevice.hpp"
 "Scene2D.cpp" "Scene2DProxy.cpp")

target_include_directories(${this_target} 
PUBLIC
	"${SOURCE_DIR}/RenderPlugin_RHI"
)

target_link_libraries(${this_target}
PRIVATE
	GameFramework
	RHI
)