cmake_minimum_required(VERSION 3.13)

set(this_target "Rendering")
project(${this_target})

# ----------------------- build library -------------------------
add_library(${this_target} SHARED)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})

target_sources (${this_target}
INTERFACE
	"RenderingSystem.h"
	"RenderingSystem.hpp"
)

# vulkan sources
target_sources (${this_target}
PRIVATE
	"VulkanImpl/VulkanContext.cpp"
	"VulkanImpl/MeshPipeline.cpp"
	"VulkanImpl/Renderer.cpp"
	#"VulkanImpl/VkBootstrap/VkBootstrap.cpp"
	"VulkanImpl/VulkanRendering.cpp" )

find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glm REQUIRED FATAL_ERROR)
find_package(vulkan-memory-allocator REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} 
	Catch2::Catch2WithMain
	Vulkan::Vulkan 
	vulkan-memory-allocator::vulkan-memory-allocator
	glm::glm
)

target_include_directories(${this_target} 
PRIVATE
	${SOURCE_DIR}/Core
)


#---------------- Link third-party libraries ---------------

include(FetchContent)
FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        main #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
target_link_libraries(${this_target} vk-bootstrap::vk-bootstrap)

# -------------- definitions ---------------------
target_compile_definitions(${this_target} 
PUBLIC
	RENDERING_BUILD
	USE_WINDOW_OUTPUT
	DATA_PATH="Data"
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})

file(REMOVE_RECURSE  ${DATA_DIR}/Shaders)
# copy shaders into bin
file(COPY VulkanImpl/Shaders DESTINATION ${DATA_DIR}/Shaders)
file(RENAME ${DATA_DIR}/Shaders/Shaders ${DATA_DIR}/Shaders/Vulkan)