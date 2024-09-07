function(hypertextcpp_BuildSharedTemplate)
    cmake_parse_arguments(
            ARG
            ""
            "NAME"
            "FILE;OUTPUT_DIR"
            ${ARGN}
    )
    if (NOT ARG_NAME)
        message(FATAL_ERROR "[hypertexcpp_BuildSharedTemplate] Argument 'NAME' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertexcpp_BuildSharedTemplate] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (ARG_FILE)
        set(SHARED_TEMPLATE_FILE ${ARG_FILE})
    else()
        set(SHARED_TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_NAME}.htcpp)
    endif()

    add_custom_command(
            OUTPUT  ${ARG_NAME}.cpp
            COMMAND hypertextcpp ${SHARED_TEMPLATE_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}.cpp -s
            DEPENDS ${SHARED_TEMPLATE_FILE}
            VERBATIM
    )

    add_library(${ARG_NAME} SHARED ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}.cpp)
    target_compile_features(${ARG_NAME} PUBLIC cxx_std_11)
    target_compile_definitions(${ARG_NAME} PUBLIC "HYPERTEXTCPP_EXPORT")
    set_target_properties(${ARG_NAME} PROPERTIES CXX_EXTENSIONS OFF)
    if (ARG_OUTPUT_DIR)
        set_target_properties(${ARG_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ARG_OUTPUT_DIR})
    endif()
endfunction()