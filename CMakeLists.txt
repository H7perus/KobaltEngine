cmake_minimum_required(VERSION 4.0)

set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(KobaltEngine)

#set(CMAKE_FIND_DEBUG_MODE TRUE)

set(Vulkan_DIR "" CACHE PATH "" FORCE)
set(Vulkan_ROOT "" CACHE PATH "" FORCE)
set(CMAKE_DISABLE_FIND_PACKAGE_Vulkan OFF)

set(ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src)

macro(kobalt_add_lib name)
	file(GLOB_RECURSE SOURCES "*.cpp" "*.h")
	file(GLOB_RECURSE SHARED "${ENGINE_SRC}/Shared/*.cpp" "${ENGINE_SRC}/Shared/*.h")

	add_library(${name} SHARED 
	${SOURCES}
	${SHARED}
	)

endmacro()


set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/lib) #time to find out if this is right
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin/lib)
link_directories("${CMAKE_BINARY_DIR}/build/bin/lib")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build/bin)




include(FetchContent)

FetchContent_Declare(
  vk-bootstrap
  GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap.git
  GIT_TAG        v1.4.342  # check their releases for the latest
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(vk-bootstrap)

set(SLANG_VERSION "2026.3.1")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(SLANG_PLATFORM "windows")
    set(SLANG_ARCHIVE_EXT "zip")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(SLANG_PLATFORM "linux")
    set(SLANG_ARCHIVE_EXT "tar.gz")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(SLANG_PLATFORM "macos")
    set(SLANG_ARCHIVE_EXT "zip")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(SLANG_ARCH "aarch64")
else()
    set(SLANG_ARCH "x86_64")
endif()

set(SLANG_URL "https://github.com/shader-slang/slang/releases/download/v${SLANG_VERSION}/slang-${SLANG_VERSION}-${SLANG_PLATFORM}-${SLANG_ARCH}.${SLANG_ARCHIVE_EXT}")

FetchContent_Declare(slang URL ${SLANG_URL})
FetchContent_MakeAvailable(slang)
find_package(slang REQUIRED PATHS "${slang_SOURCE_DIR}" NO_DEFAULT_PATH)


set(LIBS_TO_LINK
	cobaltrenderer.lib
	cobaltcore.lib
)



include_directories(${ENGINE_SRC}/Base/Include)
include_directories(${ENGINE_SRC})


find_package(OpenGL REQUIRED)         # OpenGL comes from the system
find_package(glm CONFIG REQUIRED) 

set(Vulkan_LIBRARY "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")
set(Vulkan_INCLUDE_DIR "$ENV{VULKAN_SDK}/Include")
include_directories(BEFORE "$ENV{VULKAN_SDK}/Include")
find_package(Vulkan REQUIRED)


#find_package(slang CONFIG REQUIRED)

find_package(SDL3 CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(miniz REQUIRED)


#ENGINE
add_subdirectory(${ENGINE_SRC}/Base)
add_subdirectory(${ENGINE_SRC}/Engine)

#GAME
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Game)

#ENGINE
add_subdirectory(${ENGINE_SRC}/Client ${ENGINE_SRC}/Client)
add_subdirectory(${ENGINE_SRC}/Server ${ENGINE_SRC}/Server)
add_subdirectory(${ENGINE_SRC}/GpuInterface ${ENGINE_SRC}/GpuInterface)
add_subdirectory(${ENGINE_SRC}/Launcher ${ENGINE_SRC}/Launcher)