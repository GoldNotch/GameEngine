cmake_minimum_required(VERSION 3.12)
set(this_target "glew")
project(${this_target} VERSION 2.1.0 LANGUAGES C)

if (POLICY CMP0069)
    cmake_policy(SET CMP0069 NEW)
endif()

include_directories(include)
set(${this_target}_SRC 
    "src/glew.c")

add_definitions(-DGLEW_STATIC)
add_library(${this_target} STATIC ${${this_target}_SRC})