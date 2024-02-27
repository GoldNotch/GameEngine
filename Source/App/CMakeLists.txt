cmake_minimum_required(VERSION 3.13)
set(this_target "App")
project(${this_target})

add_library(${this_target} SHARED)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

target_sources(${this_target}
INTERFACE
	"App.h"
PRIVATE
  "dllmain.cpp"
  "Game.hpp"
  "App.hpp"
)

target_include_directories(${this_target} 
PRIVATE
	${SOURCE_DIR}/Rendering
	${SOURCE_DIR}/Core
)
target_link_libraries(${this_target} Rendering Core)

find_package(assimp REQUIRED FATAL_ERROR)
target_link_libraries(${this_target} 
	Catch2::Catch2WithMain
	assimp::assimp
)

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
	-DAPP_BUILD
)

add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	#add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()