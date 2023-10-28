cmake_minimum_required(VERSION 3.12)

set(this_target "Render")
project(${this_target})


if(BUILD_WITH_GL)
message(STATUS "build gl renderer")
ADD_SUBDIRECTORY(GL_impl)
elseif(BUILD_WITH_VULKAN)
message(STATUS "build vulkan renderer")
ADD_SUBDIRECTORY(Vulkan_impl)
endif()