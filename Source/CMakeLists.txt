set(this_target "GameFramework")
set(ImplFolder "SDL")

add_library(${this_target})

target_sources(${this_target}
PUBLIC
	"GameFramework.hpp"
	"GameFramework.cpp"
PRIVATE
	"Impl/${ImplFolder}/Application.cpp"
)

target_link_libraries(${this_target}
PRIVATE
	SDL3-static
)


target_include_directories(${this_target} 
PUBLIC
	"./"
)
