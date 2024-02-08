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
	"VulkanImpl/VulkanRendering.cpp")

find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glm REQUIRED FATAL_ERROR)
find_package(vulkan-validationlayers REQUIRED FATAL_ERROR)
find_package(vk-bootstrap REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} 
	Vulkan::Vulkan 
	glm::glm
	vulkan-validationlayers::vulkan-validationlayers
	vk-bootstrap::vk-bootstrap
)

target_include_directories(${this_target} 
PRIVATE
	${SOURCE_DIR}/Core
)


#---------------- Link third-party libraries ---------------

# -------------- definitions ---------------------
target_compile_definitions(${this_target} 
PUBLIC
	RENDERING_BUILD
	USE_WINDOW_OUTPUT
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})

file(REMOVE_RECURSE  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders)
# copy shaders into bin
file(COPY VulkanImpl/Shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders)
file(RENAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders/Shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders/Vulkan)