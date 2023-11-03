cmake_minimum_required(VERSION 3.12)

set(this_target "test_dll")
project (${this_target})

add_library(${this_target} SHARED "main.cpp")