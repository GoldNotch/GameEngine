set(this_target "TriangleApp")
project(${this_target})

set(${this_target}_SOURCE_FILES
  "main.cpp"
)


add_executable(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} "Engine")
include_directories("../../source")
target_link_libraries(${this_target} Engine)