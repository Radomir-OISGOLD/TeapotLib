
cmake_minimum_required (VERSION 4.0)
set (CMAKE_CXX_STANDARD 20)
project("TeapotLib" VERSION 0.1.0.5 LANGUAGES "CXX")

set(MSVC_PATH "C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/cl.exe")

configure_file("${CMAKE_SOURCE_DIR}/version.cmake.in" "${CMAKE_SOURCE_DIR}/include/Teapot/version.hpp")
file(GLOB_RECURSE PUBLIC_SOURCE "src/*.cpp")

set(LIB_DIR "C:/_libraries")

# Find packages before creating target
set(glfw3_DIR "C:/_libraries/GLFW/lib/cmake/glfw3")
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE PUBLIC_HEADERS "include/Teapot/*.hpp")
add_library(TeapotLib STATIC ${PUBLIC_SOURCE})
add_library(TeapotLib::TeapotLib ALIAS TeapotLib)

target_include_directories(TeapotLib PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_include_directories(TeapotLib PUBLIC "C:/_libraries/vk-bootstrap/Compiled/include")

target_include_directories(
	TeapotLib 
	PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_link_directories(
	TeapotLib 
	PRIVATE 
	"${LIB_DIR}/slang/lib"
	"C:/_libraries/vk-bootstrap/Compiled/lib"
)

target_link_libraries(
	TeapotLib
	PUBLIC

	Vulkan::Vulkan
	glfw
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
