cmake_minimum_required(VERSION 3.12)
message ("generate testdll_exe")
set(this_target "testdll_exe")
project (${this_target})

add_executable(${this_target} "main.cpp")
add_dependencies(${this_target} test_dll)
include_directories(${SOURCE_DIR}/TestDLL)
target_link_libraries(${this_target} test_dll)