cmake_minimum_required(VERSION 3.13)
set(this_target "AssetLoader")
project(${this_target})

add_library(${this_target})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})

target_sources (${this_target}
PRIVATE
	"AssetLoader.cpp"

	"AssetLoader.hpp"
)

# -------------- definitions ---------------------
find_package(assimp REQUIRED FATAL_ERROR)

target_link_libraries(${this_target} 
	assimp::assimp
	Catch2::Catch2WithMain
)

target_include_directories(${this_target} 
PRIVATE
	${SOURCE_DIR}/Core
)