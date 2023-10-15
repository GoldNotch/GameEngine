cmake_minimum_required(VERSION 3.12)
if (BUILD_WITH_VULKAN)

set(this_target "RenderingVK")
project(${this_target})

set (${this_target}_SOURCE_FILES 
  "dllmain.cpp"
)

set (${this_target}_LOCAL_DEPENDENCIES
  "Rendering"
  "glfw"
)

# ----------------------- build library -------------------------
add_library(${this_target} SHARED ${this_target}_SOURCE_FILES)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} ${this_target}_LOCAL_DEPENDENCIES)
INCLUDE_DEPENDENCIES(${this_target}_LOCAL_DEPENDENCIES)

# --------------------- link libraries -------------------
find_package(Vulkan REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} Vulkan::Vulkan)

target_link_libraries(${this_target} glfw ${GLFW_LIBRARIES})

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS

)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
  "-lgdi32"
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})
else()
  message("Rendering with Vulkan - disabled")
endif()