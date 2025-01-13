cmake_minimum_required(VERSION 3.16)

project(vulkan_tutorial)

set(CMAKE_CXX_STANDARD 20)

set(Vulkan_Include_Dirs "lib/vulkan-sdk/Include")

set(VULKAN_LIBRARY "${CMAKE_SOURCE_DIR}/lib/vulkan-sdk/Lib/vulkan-1.lib")

set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019/glfw3dll.lib")
message(---------------${CMAKE_SOURCE_DIR}---------------------)
message(---------------${VULKAN_LIBRARY}---------------------)

include_directories(${Vulkan_Include_Dirs})

list(APPEND LIBS ${VULKAN_LIBRARY} ${GLFW_LIBRARY})
message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${LIBS}")

include_directories("lib/glfw/include")
include_directories("lib/glm")
include_directories("lib/stb")
add_subdirectory("lib/glm")

# set_property(GLOBAL PROPERTY DLL_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019")
# list(APPEND CMAKE_PROGRAM_PATH "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019")

add_subdirectory(DrawingTriangle)