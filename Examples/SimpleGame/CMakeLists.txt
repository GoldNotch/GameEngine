set(this_target "SimpleGame")

add_library(${this_target} SHARED
	"dllmain.cpp"
)

target_link_libraries(${this_target}
PUBLIC
	GameFramework
)