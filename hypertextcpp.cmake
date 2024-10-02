function(hypertextcpp_GenerateHeader)
    cmake_parse_arguments(
            ARG
            ""
            "NAME"
            "FILE;CLASS_NAME;OUTPUT_DIR"
            ${ARGN}
    )
    if (NOT ARG_NAME)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Argument 'NAME' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (ARG_FILE)
        set(TEMPLATE_FILE ${ARG_FILE})
    else()
        set(TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_NAME}.htcpp)
    endif()

    if (ARG_OUTPUT_DIR)
        set(OUTPUT_DIR ${ARG_OUTPUT_DIR})
    else()
        set(OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    endif()

    if (ARG_CLASS_NAME)
        add_custom_command(
                OUTPUT  ${ARG_NAME}.h
                COMMAND hypertextcpp ${TEMPLATE_FILE} -o ${OUTPUT_DIR}/${ARG_NAME}.h -c ${ARG_CLASS_NAME}
                DEPENDS ${TEMPLATE_FILE}
                VERBATIM
        )
    else()
        add_custom_command(
                OUTPUT  ${ARG_NAME}.h
                COMMAND hypertextcpp ${TEMPLATE_FILE} -o ${OUTPUT_DIR}/${ARG_NAME}.h
                DEPENDS ${TEMPLATE_FILE}
                VERBATIM
        )
    endif()
endfunction()

function(hypertextcpp_BuildSharedLibrary)
    cmake_parse_arguments(
            ARG
            ""
            "NAME"
            "FILE;OUTPUT_DIR"
            ${ARGN}
    )
    if (NOT ARG_NAME)
        message(FATAL_ERROR "[hypertextcpp_BuildSharedLibrary] Argument 'NAME' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertextcpp_BuildSharedLibrary] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (ARG_FILE)
        set(TEMPLATE_FILE ${ARG_FILE})
    else()
        set(TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_NAME}.htcpp)
    endif()

    add_custom_command(
            OUTPUT  ${ARG_NAME}.cpp
            COMMAND hypertextcpp ${TEMPLATE_FILE} -o ${CMAKE_CURRENT_BINARY_DIR}/${ARG_NAME}.cpp -s
            DEPENDS ${TEMPLATE_FILE}
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