file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(DrawTriangle ${SOURCES})
target_link_libraries(DrawTriangle ${LIBS} glm::glm)