set(this_target "Core")

add_library(${this_target} INTERFACE)

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