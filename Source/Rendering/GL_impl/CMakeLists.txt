cmake_minimum_required(VERSION 3.12)

set(this_target "RenderingGL")
project(${this_target})

set (${this_target}_SOURCE_FILES 
)

set (${this_target}_LOCAL_DEPENDENCIES
  "Rendering"
  "glfw"
  "glew"
)

# ----------------------- build library -------------------------
add_library(${this_target} SHARED ${this_target}_SOURCE_FILES)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} ${this_target}_LOCAL_DEPENDENCIES)
INCLUDE_DEPENDENCIES(${this_target}_LOCAL_DEPENDENCIES)

# --------------------- link libraries -------------------

target_link_libraries(${this_target} glew)
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
  "-lopengl32"
  "-lgdi32"
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})