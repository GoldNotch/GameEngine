cmake_minimum_required(VERSION 3.13)

set(this_target "Laucher")
project(${this_target})

add_executable(${this_target})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

target_sources(${this_target}
PUBLIC
  "main.cpp"
PRIVATE
  "Process.cpp"
  "Process.hpp"
  #"glew/glew.c"

)

target_include_directories(${this_target} 
PRIVATE 
	${SOURCE_DIR}/App
	${SOURCE_DIR}/Core
	${SOURCE_DIR}/Rendering
)
target_link_libraries(${this_target} PRIVATE App)

find_package(glfw3 REQUIRED)
target_link_libraries(${this_target} PRIVATE glfw)
target_include_directories(${this_target} PRIVATE ${glfw3_INCLUDE_DIRS})

#target_include_directories(${this_target} PRIVATE glew)

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()