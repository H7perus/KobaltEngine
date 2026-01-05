#TODO: Mister Claudius says this will only run on manual cmake rerun due to GLOB_RECURSE, i.e. new files ain't detected. Can't be bothered to check that rn
file(GLOB_RECURSE SOURCES "*.cpp" "*.h")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(Server SHARED 
	${SOURCES}
)

# Specify the include directories so that other projects can use this module's headers
target_include_directories(Server PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(Server PRIVATE SERVER_DLL_EXPORTS)

set_target_properties(Server PROPERTIES FOLDER Engine)