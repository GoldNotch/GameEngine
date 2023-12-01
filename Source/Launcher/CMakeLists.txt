set(this_target "Laucher")
project(${this_target})

set(${this_target}_SOURCE_FILES
  "main.cpp"
)

add_executable(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

add_dependencies(${this_target} EngineLib)
include_directories(${SOURCE_DIR}/EngineLib)
target_link_libraries(${this_target} PRIVATE EngineLib)
find_package(glfw3 REQUIRED)
target_link_libraries(${this_target} PRIVATE glfw)
target_include_directories(${this_target} PRIVATE ${glfw3_INCLUDE_DIRS})

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()