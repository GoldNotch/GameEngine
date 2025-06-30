set(this_target "SimpleGame")

add_library(${this_target} SHARED
	"dllmain.cpp"
)

target_include_directories(${this_target}
	PRIVATE
		${SOURCE_DIR}
)