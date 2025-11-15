file(GLOB_RECURSE SOURCES *.cpp *.h)
add_executable(TemplateDev ${SOURCES})
target_link_libraries(TemplateDev ${LIBS} glm::glm)

set_target_properties(TemplateDev PROPERTIES 
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    # RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)