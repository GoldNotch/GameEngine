cmake_minimum_required(VERSION 3.12)

set(this_target "Laucher")
project(${this_target})

set(${this_target}_SOURCE_FILES
  "main.cpp"
  "Process.cpp"
  #"glew/glew.c"
)

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

target_include_directories(${this_target} PRIVATE ${SOURCE_DIR}/App)
target_include_directories(${this_target} PRIVATE ${SOURCE_DIR}/EngineLib)
target_link_libraries(${this_target} PRIVATE App)

find_package(glfw3 REQUIRED)
target_link_libraries(${this_target} PRIVATE glfw)
target_include_directories(${this_target} PRIVATE ${glfw3_INCLUDE_DIRS})

target_include_directories(${this_target} PRIVATE glew)

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()