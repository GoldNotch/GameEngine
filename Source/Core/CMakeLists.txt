cmake_minimum_required(VERSION 3.13)

set(this_target "Core")
project(${this_target})

add_library(${this_target} INTERFACE)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

target_sources(${this_target}
INTERFACE
	"Formatter.hpp"
	"Logging.hpp"
	"Storage.hpp"
	"TypeMapping.hpp"
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()


add_subdirectory(Tests)