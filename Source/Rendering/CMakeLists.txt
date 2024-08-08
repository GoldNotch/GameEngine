cmake_minimum_required(VERSION 3.13)
set(this_target "Rendering")
project(${this_target})

add_library(${this_target} SHARED)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})

# ----------------------- build library -------------------------

target_compile_definitions(${this_target} 
PUBLIC
	RENDERING_BUILD
	USE_WINDOW_OUTPUT
	DATA_PATH="Data"
)

target_sources (${this_target}
INTERFACE
	"RenderingSystem.h"
	"Scene.h"  
)

add_subdirectory(AssetLoader)
add_subdirectory(VulkanImpl)
#add_subdirectory(DirectXImpl)

target_link_libraries(${this_target} 
	Catch2::Catch2WithMain
	AssetLoader
)

target_include_directories(${this_target} 
PRIVATE
	${SOURCE_DIR}/Core
	AssetLoader/
)

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

