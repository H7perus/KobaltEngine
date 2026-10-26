kobalt_add_lib(Engine)
set_target_properties(Engine PROPERTIES FOLDER Engine)

# Specify the include directories so that other projects can use this module's headers


target_compile_definitions(Engine PRIVATE ENGINE_DLL_EXPORTS)

find_path(TINYGLTF_INCLUDE_DIRS "tiny_gltf.h")
target_include_directories(Engine PRIVATE ${TINYGLTF_INCLUDE_DIRS})

target_link_libraries(Engine PRIVATE Base)


#target_link_libraries(Engine PRIVATE SDL3::SDL3 imgui::imgui)