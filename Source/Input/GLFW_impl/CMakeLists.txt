cmake_minimum_required(VERSION 3.12)

set(this_target "Input")
project(${this_target})

set (${this_target}_SOURCE_FILES 
  "Window.cpp"
)

set (${this_target}_LOCAL_DEPENDENCIES
  glfw
)

set (${this_target}_ADDITIONAL_INCLUDES
  ${SOURCE_DIR}/Input
)

# ----------------------- build library -------------------------
add_library(${this_target} SHARED ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} ${${this_target}_LOCAL_DEPENDENCIES})
INCLUDE_DEPENDENCIES(${this_target} PRIVATE ${${this_target}_LOCAL_DEPENDENCIES})

foreach(_path ${${this_target}_ADDITIONAL_INCLUDES})
	target_include_directories(${this_target} PRIVATE ${_path})
endforeach()

# --------------------- link libraries -------------------
target_link_libraries(${this_target} glfw ${GLFW_LIBRARIES})

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
	"-DBUILD_DLL"
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