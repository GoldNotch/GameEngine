cmake_minimum_required(VERSION 3.12)
include(CMakePrintHelpers)
include(Utils.cmake)

set(this_target "Root")
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

# -------------------- Install conan packages ----------------------
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# copy conanfile.txt into cache
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conanfile.txt")
    message(STATUS "Copy conanfile.txt into CMake cache")
    configure_file(${PROJECT_SOURCE_DIR}/conanfile.txt ${CMAKE_BINARY_DIR}/conanfile.txt COPYONLY)
endif()

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    PROFILE ${PROJECT_SOURCE_DIR}/ConanProfiles/windows)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/build/generators)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/build/generators)

# ------------------------- Build projects ------------------------------
ADD_SUBDIRECTORY(${SOURCE_DIR}/EngineLib)
ADD_SUBDIRECTORY(${SOURCE_DIR}/Launcher)
ADD_SUBDIRECTORY(${SOURCE_DIR}/DummyApp)
