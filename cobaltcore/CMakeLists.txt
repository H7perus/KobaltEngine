file(GLOB SOURCES "src/*.cpp" "src/*.h")

add_library(cobaltcore SHARED 
	${SOURCES}
)

# Specify the include directories so that other projects can use this module's headers
target_include_directories(cobaltcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_definitions(cobaltcore PRIVATE CORE_DLL_EXPORTS)

set_target_properties(cobaltcore PROPERTIES FOLDER Engine)