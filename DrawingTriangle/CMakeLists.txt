file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(DrawTriangle ${SOURCES})
target_link_libraries(DrawTriangle ${LIBS} glm::glm)

set_target_properties(DrawTriangle PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    # RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_command(
    TARGET DrawTriangle
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019/glfw3.dll"
    $<TARGET_FILE_DIR:DrawTriangle>
    COMMENT "Copying glfw3.dll to the output directory of DrawTriangle"
)