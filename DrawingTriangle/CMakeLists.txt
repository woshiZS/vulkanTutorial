file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(DrawTriangle ${SOURCES})
target_link_libraries(DrawTriangle ${LIBS} glm::glm)

set_target_properties(DrawTriangle PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    # RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)