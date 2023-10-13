cmake_minimum_required(VERSION 3.12)

option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_WITH_VULKAN "build with vulkan" OFF)
option(BUILD_WITH_GL "build with open_gl" ON)

# returns all subdirectories in directory
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
cmake_policy(SET CMP0060 NEW)
set(this_target "Engine")
project(${this_target})

set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)

# ------------ process all dependencies ----------------
SUBDIRLIST(ALL_DEPENDENCIES ${THIRD_PARTY_DIR})
FOREACH(lib ${ALL_DEPENDENCIES})
  ADD_SUBDIRECTORY(${THIRD_PARTY_DIR}/${lib})
  include_directories(${THIRD_PARTY_DIR}/${lib}/include)
ENDFOREACH()
  

# -------------- all source files ----------------
include_directories(include)
set (${this_target}_GL_SOURCE_FILES 
  "source/Window_GLFW.cpp"  
  "source/Renderer_GL.cpp"
)

set(${this_target}_SOURCE_FILES
  "source/Renderer.cpp"
  "source/Window.cpp"
)

# --------------- build app ------------------
add_library(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} ${ALL_DEPENDENCIES})

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
  "-DGLEW_STATIC"
)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()

# -------------- link third-party libraries ------------------
target_link_libraries(${this_target} glew)
# glfw is special so link it separatelly
target_link_libraries(${this_target} glfw ${GLFW_LIBRARIES})

# -------------- link system libraries -----------

set(${this_target}_SYSTEM_LIBRARIES 
  "-lopengl32"
  "-lgdi32"
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})



if (BUILD_EXAMPLES)
    message(STATUS "start build examples")
    set(EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/Examples)
    SUBDIRLIST(ALL_EXAMPLES ${EXAMPLES_DIR})
    FOREACH(example ${ALL_EXAMPLES})
        ADD_SUBDIRECTORY(${EXAMPLES_DIR}/${example})
    ENDFOREACH()
endif()
