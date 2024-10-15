# CMakeLists.txt for cobaltcore
# project(CobaltCore)

# Add the library (cobaltcore will be built as a DLL)
add_library(cobaltcore SHARED 
	./cobaltcore.cpp
)

# Specify the include directories so that other projects can use this module's headers
target_include_directories(cobaltcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(cobaltcore PROPERTIES FOLDER Engine)