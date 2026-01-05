# Add the library (Launcher is an exe)

file(GLOB_RECURSE SOURCES "*.cpp" "*.h")


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})


add_executable(Launcher
	${SOURCES}
)
set_target_properties(Launcher PROPERTIES FOLDER Engine)

target_include_directories(Launcher PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)

find_package(SDL3 CONFIG REQUIRED)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/engine/vcpkg_installed/x64-windows/share/glew")
find_package(GLEW CONFIG REQUIRED)


target_link_libraries(Launcher PRIVATE Base)
target_link_libraries(Launcher PRIVATE Engine)
target_link_libraries(Launcher PRIVATE Client)
