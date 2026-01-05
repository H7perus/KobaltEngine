kobalt_add_lib(Client)

target_link_libraries(Client PRIVATE Base)
target_link_libraries(Client PRIVATE Engine)
target_link_libraries(Client PRIVATE GpuInterface)
target_link_libraries(Client PUBLIC SDL3::SDL3)
target_link_libraries(Client PRIVATE ${Vulkan_LIBRARIES})

# Specify the include directories so that other projects can use this module's headers
target_include_directories(Client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Client PRIVATE CLIENT_DLL_EXPORTS)

set_target_properties(Client PROPERTIES FOLDER Engine)