#TODO: Mister Claudius says this will only run on manual cmake rerun due to GLOB_RECURSE, i.e. new files ain't detected. Can't be bothered to check that rn
file(GLOB_RECURSE SOURCES "*.cpp" "*.h" "*.ixx")

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SOURCES})

add_library(Base SHARED 
	${SOURCES}
)


set_target_properties(Base PROPERTIES
    CXX_SCAN_FOR_MODULES ON
)

# Specify the include directories so that other projects can use this module's headers
target_include_directories(Base PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_definitions(Base PRIVATE BASE_DLL_EXPORTS)

set_target_properties(Base PROPERTIES FOLDER Engine)