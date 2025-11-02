set(test_target ${this_target}_Tests)

add_executable(${test_target})
target_sources(${test_target}
PUBLIC
	"Test_StaticString.cpp"
	"Test_Storage.cpp"
	"Test_Files.cpp"
)

target_link_libraries(${test_target}  PRIVATE
	Catch2::Catch2WithMain
	GameFramework
)