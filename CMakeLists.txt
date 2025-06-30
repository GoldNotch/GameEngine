cmake_minimum_required(VERSION 3.13)
include(Utils.cmake)

project(GameFramework C CXX)

# ------------------ options and settings --------------

option(BUILD_EXAMPLES "Build example programs" ON)

set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/Source)
set(APP_DIR ${PROJECT_SOURCE_DIR}/Build/${CMAKE_BUILD_TYPE})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${APP_DIR}/Obj)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${APP_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_DIR})

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_C_STANDATD 17)
cmake_policy(SET CMP0060 NEW)

# ------------------------- Build projects ------------------------------
add_subdirectory(ThirdParty)
add_subdirectory(Source)
if(${BUILD_EXAMPLES})
	add_subdirectory(Examples)
endif()
