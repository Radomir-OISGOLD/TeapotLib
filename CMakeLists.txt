
cmake_minimum_required (VERSION 4.0)
set (CMAKE_CXX_STANDARD 20)
project("TeapotLib" VERSION 0.1.0.5 LANGUAGES "CXX")

configure_file("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/include/Teapot/common/version.hpp")
file(GLOB_RECURSE PUBLIC_SOURCE 
	"src/core/*.cpp"
	"src/platform/*.cpp"
	"src/common/*.cpp"
	"src/pipeline/*.cpp"
)

set(LIB_DIR "C:/.libraries")

# Find packages before creating target
set(glfw3_DIR "${LIB_DIR}/GLFW/lib/cmake/glfw3/")
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE PUBLIC_HEADERS 
	"include/Teapot/core/*.hpp"
	"include/Teapot/platform/*.hpp"
	"include/Teapot/common/*.hpp"
	"include/Teapot/pipeline/*.hpp"
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
	PRIVATE

	Vulkan::Vulkan
	glfw3
)

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
