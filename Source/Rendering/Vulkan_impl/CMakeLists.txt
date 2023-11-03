set(THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)
BUILD_DEPENDENCIES(${THIRD_PARTY_DIR} ALL_DEPENDENCIES)

set (${this_target}_SOURCE_FILES 
)

# ----------------------- build library -------------------------
add_library(${this_target} SHARED ${this_target}_SOURCE_FILES)
set_property(TARGET ${this_target} PROPERTY CXX_STANDARD ${GLOBAL_CXX_STANDARD})  
set_property(TARGET ${this_target} PROPERTY C_STANDARD ${GLOBAL_C_STANDARD})  
target_include_directories(${this_target} PRIVATE "../")

add_dependencies(${this_target} ${ALL_DEPENDENCIES})
INCLUDE_DEPENDENCIES(${this_target} PRIVATE ${THIRD_PARTY_DIR})

# --------------------- link third-party libraries -------------------

target_link_libraries(${this_target} glew)

# -------------- definitions ---------------------
set(${this_target}_COMPILE_DEFINITIONS
	"-DBUILD_DLL"
)
add_definitions(${${this_target}_COMPILE_DEFINITIONS})

if (MSVC)
	add_compile_definitions(_HAS_ITERATOR_DEBUGGING=0) # disable iterator debugging in MSVC
endif()

# -------------- link system libraries -----------
set(${this_target}_SYSTEM_LIBRARIES 
)
target_link_libraries(${this_target} ${${this_target}_SYSTEM_LIBRARIES})