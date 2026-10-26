file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.ixx" "*.slang")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(Renderer SHARED 
	${SOURCES}
)

target_compile_definitions(Renderer PRIVATE RENDERER_DLL_EXPORTS VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1)
set_target_properties(Renderer PROPERTIES FOLDER Engine)

set_target_properties(Renderer PROPERTIES
    CXX_SCAN_FOR_MODULES ON
)

# Specify the include directories so that other projects can use this module's headers

target_link_libraries(Renderer PRIVATE Base)
target_link_libraries(Renderer PRIVATE GpuInterface)
target_link_libraries(Renderer PRIVATE Engine)

target_link_libraries(Renderer PUBLIC Vulkan::Vulkan vk-bootstrap::vk-bootstrap slang::slang)

target_include_directories(Renderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_include_directories(Renderer PUBLIC 
    ${vk-bootstrap_SOURCE_DIR}/src
)






