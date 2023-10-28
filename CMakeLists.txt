cmake_minimum_required(VERSION 3.12)
include(CMakePrintHelpers)
include(Utils.cmake)

set(this_target "Engine")
project(${this_target})

# ------------------ options and settings --------------

option(BUILD_EXAMPLES "Build example programs" ON)
option(BUILD_WITH_VULKAN "build with vulkan" OFF)
option(BUILD_WITH_GL "build with OpenGL" OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(GLOBAL_C_STANDARD 17)
set(GLOBAL_CXX_STANDARD 20)
cmake_policy(SET CMP0060 NEW)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source)

# ------------- build all libraries -------------------
SUBDIRLIST(ALL_SUBPROJECTS ${SOURCE_DIR})
FOREACH(subproject ${ALL_SUBPROJECTS})
  message(STATUS "generate project - ${subproject}")
  ADD_SUBDIRECTORY(${SOURCE_DIR}/${subproject})
ENDFOREACH()

# ----------------- build all executables -------------------
if (BUILD_EXAMPLES)
    message(STATUS "start build examples")
    set(EXAMPLES_DIR ${PROJECT_SOURCE_DIR}/Examples)
    SUBDIRLIST(ALL_EXAMPLES ${EXAMPLES_DIR})
    FOREACH(example ${ALL_EXAMPLES})
        message("generate example - ${example}")
        ADD_SUBDIRECTORY(${EXAMPLES_DIR}/${example})
    ENDFOREACH()
endif()
