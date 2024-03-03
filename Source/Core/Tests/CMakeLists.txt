cmake_minimum_required(VERSION 3.13)
set(test_target ${this_target}_Tests)
project(test_target)

add_executable(${test_target})
target_sources(${test_target}
PUBLIC
	"Test_StaticString.cpp"
 "Test_Storage.cpp")
target_include_directories(${test_target}  PRIVATE
	"../"
)

target_link_libraries(${test_target}  PRIVATE
	Catch2::Catch2WithMain
)