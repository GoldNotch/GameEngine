cmake_minimum_required(VERSION 3.12)

set(this_target "Core")
project(${this_target})

set (${this_target}_SOURCE_FILES 
)

set (${this_target}_DEPENDENCIES
)


# ----------------------- build library -------------------------
add_library(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
if (NOT ${${this_target}_DEPENDENCIES} STREQUAL "")
add_dependencies(${this_target} ${${this_target}_DEPENDENCIES})
endif()

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
  #"-DGLEW_STATIC"
)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()

