# vulkan sources
target_sources (${this_target}
PRIVATE
	"VulkanContext.cpp"
	"Pipelines/MeshPipeline.cpp"
	"Renderer.cpp"
	"VulkanRendering.cpp"  
	"MemoryManager.cpp" 
	"Utils/Pipeline.cpp"
	"Utils/PipelineBuilder.cpp"

	"VulkanContext.hpp"
	"Renderer.hpp"
	"MemoryManager.hpp"
	"Utils/Pipeline.hpp"
	"Utils/PipelineBuilder.hpp"
	"Utils/ShaderCompiler.hpp"
)

# -------------- definitions ---------------------
find_package(Vulkan REQUIRED FATAL_ERROR)
find_package(glm REQUIRED FATAL_ERROR)
find_package(vulkan-memory-allocator REQUIRED FATAL_ERROR)

target_link_libraries(${this_target} 
	Vulkan::Vulkan 
	vulkan-memory-allocator::vulkan-memory-allocator
	glm::glm
)

#---------------- Link third-party libraries ---------------

FetchContent_Declare(
    fetch_vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG        main #suggest using a tag so the library doesn't update whenever new commits are pushed to a branch
)
FetchContent_MakeAvailable(fetch_vk_bootstrap)
target_link_libraries(${this_target} vk-bootstrap::vk-bootstrap)

file(REMOVE_RECURSE  ${DATA_DIR}/Shaders)
# copy shaders into bin
file(COPY Shaders DESTINATION ${DATA_DIR}/Shaders)
file(RENAME ${DATA_DIR}/Shaders/Shaders ${DATA_DIR}/Shaders/Vulkan)