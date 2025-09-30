# Add the library (module1 will be built as a DLL)
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")

add_library(cobaltrenderer SHARED 
	${SOURCES}
)


find_package(SDL3 CONFIG REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engine/vcpkg_installed/x64-windows/share/glew")
find_package(GLEW CONFIG REQUIRED)


# Specify the include directories so that other projects can use this module's headers
target_include_directories(cobaltrenderer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(cobaltrenderer PRIVATE RENDERER_DLL_EXPORTS)

set_target_properties(cobaltrenderer PROPERTIES FOLDER Engine)