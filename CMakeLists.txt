cmake_minimum_required(VERSION 3.12)
MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
      LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
cmake_policy(SET CMP0060 NEW)
set(this_target "test")
project(${this_target})


set(THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/ThirdParty)

# ------------ process all dependencies ----------------
SUBDIRLIST(ALL_DEPENDENCIES ${THIRD_PARTY_DIR})
FOREACH(lib ${ALL_DEPENDENCIES})
  ADD_SUBDIRECTORY(${THIRD_PARTY_DIR}/${lib})
  include_directories(${THIRD_PARTY_DIR}/${lib}/include)
ENDFOREACH()
  
set(${this_target}_SOURCE_FILES
  "source/main.cpp"
  "source/GLFW_Window.cpp"  
  "source/Renderer.cpp"
  "source/GL_Renderer.cpp"
)

# build app
add_executable(${this_target} ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD 20)  
add_dependencies(${this_target} ${ALL_DEPENDENCIES})

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
  "-DGLEW_STATIC"
)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

# -------------- link third-party libraries ------------------
target_link_libraries(${this_target} glew)
# glfw is special so link it separatelly
target_link_libraries(${this_target} glfw ${GLFW_LIBRARIES})

# -------------- link sysmem libraries -----------

set(${this_target}_SYSTEM_LIBRARIES 
  "-lopengl32"
  "-lgdi32"
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})

