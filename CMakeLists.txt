cmake_minimum_required(VERSION 3.16)

project(vulkan_tutorial)

set(CMAKE_CXX_STANDARD 17)

set(Vulkan_Include_Dirs "lib/vulkan-sdk/Include")

set(VULKAN_LIBRARY "lib/vulkan-sdk/Lib/vulkan-1.lib")

set(GLFW_LIBRARY "lib/glfw/lib-vc2019/glfw3dll.lib")

include_directories(${Vulkan_Include_Dirs})

list(APPEND LIBS ${VULKAN_LIBRARY} ${GLFW_LIBRARY})
message(STATUS "Vulkan Include dirs: ${Vulkan_INCLUDE_DIRS} and libs: ${LIBS}")

include_directories("lib/glfw/include")
include_directories("lib/glm")
add_subdirectory("lib/glm")

add_subdirectory(DrawingTriangle)