set(this_target "GameFramework")

add_library(${this_target} SHARED)

target_sources(${this_target}
PUBLIC
	"GameFramework.hpp"
PRIVATE
	"GameFramework_def.h"
	"GameFramework.cpp"
	"Core/Formatter.hpp"
	"Core/Storage.hpp"
	"Core/TypeMapping.hpp"
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${this_target}
PRIVATE
	-DGAME_FRAMEWORK_BUILD
)

target_include_directories(${this_target}
	PUBLIC
		"${SOURCE_DIR}/GameFramework"
)
add_subdirectory(Tests)