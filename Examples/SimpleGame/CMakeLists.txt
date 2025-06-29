set(this_target "SimpleGame")

add_executable(${this_target}
	"main.cpp"
)

target_link_libraries(${this_target}
	PRIVATE
		GameFramework
)