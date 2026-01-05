file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.ixx" "*.slang")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(GpuInterface SHARED 
	${SOURCES}
)


set_target_properties(GpuInterface PROPERTIES
    CXX_SCAN_FOR_MODULES ON
)

# Specify the include directories so that other projects can use this module's headers
target_include_directories(GpuInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(GpuInterface PRIVATE Base)
target_link_libraries(GpuInterface PUBLIC ${Vulkan_LIBRARIES} vk-bootstrap::vk-bootstrap slang::slang)


target_link_libraries(GpuInterface PUBLIC SDL3::SDL3)

target_compile_definitions(GpuInterface PRIVATE RENDERER_DLL_EXPORTS)

set_target_properties(GpuInterface PROPERTIES FOLDER Engine)