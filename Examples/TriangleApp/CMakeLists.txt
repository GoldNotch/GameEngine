set(this_target "TriangleApp")
project(${this_target})

set(${this_target}_SOURCE_FILES
  "main.cpp"
)


add_executable(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

add_dependencies(${this_target} Application)
include_directories(${SOURCE_DIR}/Application)
target_link_libraries(${this_target} Application)

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()