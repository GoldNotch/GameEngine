cmake_minimum_required(VERSION 3.12)

set(this_target "EngineLib")
project(${this_target})

set (${this_target}_SOURCE_FILES
	"Engine.cpp"
	"Rendering/VulkanImpl/VulkanRendering.cpp"
	"Rendering/VulkanImpl/MeshPipeline.cpp"
	"Rendering/VulkanImpl/Renderer.cpp"
	#"Rendering/VulkanImpl/VkBootstrap/VkBootstrap.cpp"
)

# ----------------------- build library -------------------------
add_library(${this_target} SHARED ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})

find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} Vulkan::Vulkan)
find_package(glm REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} glm::glm)
find_package(vulkan-validationlayers REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} vulkan-validationlayers::vulkan-validationlayers)
find_package(vk-bootstrap REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} vk-bootstrap::vk-bootstrap)


#---------------- Link third-party libraries ---------------

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
	-DENGINE_BUILD
	-DUSE_WINDOW_OUTPUT
)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})

file(REMOVE_RECURSE  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders)
# copy shaders into bin
file(COPY Rendering/VulkanImpl/Shaders DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders)
file(RENAME ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders/Shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Shaders/Vulkan)