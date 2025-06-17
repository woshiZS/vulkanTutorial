file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(hppTriangle ${SOURCES})
target_link_libraries(hppTriangle ${LIBS} glm::glm)

set_target_properties(hppTriangle PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    # RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

add_custom_command(
    TARGET hppTriangle
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${CMAKE_SOURCE_DIR}/lib/glfw/lib-vc2019/glfw3.dll"
    $<TARGET_FILE_DIR:hppTriangle>
    COMMENT "Copying glfw3.dll to the output directory of DrawTriangle"
)