set(this_target "GameSystems")

add_library(${this_target})

target_sources(${this_target}
PUBLIC
	"GameSystems.hpp"
PRIVATE
	"GameSystems_def.h"
	"GameSystems.cpp"

	"GameInstance/GameInstance.hpp"
	"GameInstance/GameInstance.cpp"
	"GameInstance/GameLibrary.hpp"
	"GameInstance/GameLibrary.cpp"
)

if (MSVC)
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

target_compile_definitions(${this_target}
PRIVATE
	-DGAME_SYSTEMS_BUILD
)

target_include_directories(${this_target}
	PUBLIC
		"${SOURCE_DIR}/GameSystems"
)

target_link_libraries(${this_target}
PRIVATE
	dylib
	GameFramework
)