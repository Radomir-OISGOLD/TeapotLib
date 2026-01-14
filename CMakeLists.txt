
cmake_minimum_required (VERSION 4.0)
set (CMAKE_CXX_STANDARD 20)
project("TeapotLib" VERSION 0.1.0.5 LANGUAGES "CXX")

configure_file("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/include/Teapot/common/version.hpp")
file(GLOB_RECURSE PUBLIC_SOURCE
	"src/core/*.cpp"
	"src/platform/*.cpp"
	"src/common/*.cpp"
	"src/pipeline/*.cpp"
	"src/ui/*.cpp"
)

set(LIB_DIR "C:/.libraries")

# Find packages before creating target
set(glfw3_DIR "${LIB_DIR}/GLFW/lib/cmake/glfw3/")
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

option(TEAPOTLIB_BUILD_TESTS "Build the tests for TeapotLib" ON)
if(TEAPOTLIB_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

file(GLOB_RECURSE PUBLIC_HEADERS
	"include/Teapot/core/*.hpp"
	"include/Teapot/platform/*.hpp"
	"include/Teapot/common/*.hpp"
	"include/Teapot/pipeline/*.hpp"
	"include/Teapot/ui/*.hpp"
)
add_library(TeapotLib STATIC ${PUBLIC_SOURCE} "${CMAKE_CURRENT_SOURCE_DIR}/deps/vkb/VkBootstrap.cpp")

target_include_directories(
	TeapotLib 
	PRIVATE 
	"${CMAKE_CURRENT_SOURCE_DIR}/include" 
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/vkb"
	"${CMAKE_CURRENT_SOURCE_DIR}/deps/external_includes"
)

target_include_directories(
	TeapotLib 
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/vkb>
	$<INSTALL_INTERFACE:include>
	$<INSTALL_INTERFACE:include/deps/vkb>
)

target_link_directories(
	TeapotLib 
	PRIVATE 
	"${LIB_DIR}/slang/lib"
)

target_link_libraries(
	TeapotLib
	PUBLIC

	Vulkan::Vulkan
	glfw
)

option(TEAPOTLIB_BUILD_TESTS "Build the tests for TeapotLib" ON)
if(TEAPOTLIB_BUILD_TESTS)
    add_subdirectory(tests)
endif()

enable_testing()

target_include_directories(
    TeapotLib
    PUBLIC
    $<TARGET_PROPERTY:Vulkan::Vulkan,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:glfw,INTERFACE_INCLUDE_DIRECTORIES>
)

install(
	TARGETS TeapotLib
	EXPORT TeapotLibTargets
	ARCHIVE DESTINATION "lib"
)

install(
	DIRECTORY "include/" 
	DESTINATION include
)
install(
	DIRECTORY "deps/vkb" 
	DESTINATION include/deps
)

install(
	EXPORT TeapotLibTargets
	FILE TeapotLibTargets.cmake
	NAMESPACE TeapotLib::
	DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/TeapotLib"
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
	"TeapotLibConfig.cmake.in" 
	"${CMAKE_CURRENT_BINARY_DIR}/TeapotLibConfig.cmake"
	INSTALL_DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/TeapotLib/"
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/TeapotLibConfigVersion.cmake"
  VERSION ${VERSION}
  COMPATIBILITY SameMajorVersion
)

install(
	FILES "${CMAKE_CURRENT_BINARY_DIR}/TeapotLibConfig.cmake"
		  "${CMAKE_CURRENT_BINARY_DIR}/TeapotLibConfigVersion.cmake"
	DESTINATION "${CMAKE_INSTALL_PREFIX}/cmake/TeapotLib"
)
