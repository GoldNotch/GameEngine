set(this_target "DummyApp")
project(${this_target})

set(${this_target}_SOURCE_FILES
  "dllmain.cpp"
)


add_library(${this_target} SHARED ${${this_target}_SOURCE_FILES})
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  

#add_dependencies(${this_target} EngineLib)
include_directories(${SOURCE_DIR}/EngineLib)
#target_link_libraries(${this_target} EngineLib)

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()