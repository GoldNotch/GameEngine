set(this_target "GameFramework")

add_library(${this_target} SHARED)

target_sources(${this_target}
PUBLIC
	"GameFramework.hpp"
	"Resources/Resource.hpp"
PRIVATE
	"GameFramework_def.h"
	"GameFramework.cpp"
	"Core/Formatter.hpp"
	"Core/Storage.hpp"
	"Core/TypeMapping.hpp"
	"Resources/StaticMesh.cpp" 
	"Resources/StaticMesh.hpp" 
	"Resources/ResourceLoader.cpp"
	"Resources/ResourceLoader.hpp"
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${this_target}
PRIVATE
	-DGAME_FRAMEWORK_BUILD
)

target_link_libraries(${this_target}
PRIVATE
	assimp
)

target_include_directories(${this_target}
	PUBLIC
		"${SOURCE_DIR}/GameFramework"
)
add_subdirectory(Tests)