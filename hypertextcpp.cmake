function(hypertextcpp_GenerateHeader)
    cmake_parse_arguments(
            ARG
            ""
            "TEMPLATE_FILE;CLASS_NAME;OUTPUT_DIR"
            ""
            ${ARGN}
    )
    if (NOT ARG_TEMPLATE_FILE)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Argument 'TEMPLATE_FILE' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT IS_ABSOLUTE ${ARG_TEMPLATE_FILE})
        set(TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_TEMPLATE_FILE})
    else()
        set(TEMPLATE_FILE ${ARG_TEMPLATE_FILE})
    endif()

    _hypertextcppImpl_StringAfterLast(${TEMPLATE_FILE} / TEMPLATE_NAME)
    _hypertextcppImpl_StringBeforeLast(${TEMPLATE_NAME} "." TEMPLATE_NAME)


    if (ARG_OUTPUT_DIR)
        if (NOT IS_ABSOLUTE ${ARG_OUTPUT_DIR})
            set(OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.h)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${CMAKE_CURRENT_SOURCE_DIR}/${ARG_OUTPUT_DIR}")
        else()
            set(OUTPUT_FILE ${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.h)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${ARG_OUTPUT_DIR}")
        endif()
    else()
        set(OUTPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${TEMPLATE_NAME}.h)
        set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    if (ARG_CLASS_NAME)
        set(CLASS_NAME_CMDLINE_PARAM "-className=${ARG_CLASS_NAME}")
    endif()
    add_custom_command(
            OUTPUT  ${OUTPUT_FILE}
            COMMAND hypertextcpp generateHeaderOnly ${TEMPLATE_FILE} ${OUTPUT_DIR_CMDLINE_PARAM} ${CLASS_NAME_CMDLINE_PARAM}
            DEPENDS ${TEMPLATE_FILE}
            VERBATIM
    )
endfunction()

function(hypertextcpp_GenerateHeaderAndSource)
    cmake_parse_arguments(
            ARG
            ""
            "TEMPLATE_FILE;CLASS_NAME;OUTPUT_DIR;CONFIG_CLASS_NAME"
            ""
            ${ARGN}
    )
    if (NOT ARG_TEMPLATE_FILE)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Argument 'TEMPLATE_FILE' is missing")
    endif()
    if (NOT ARG_CONFIG_CLASS_NAME)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Argument 'CONFIG_CLASS_NAME' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertextcpp_GenerateHeader] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT IS_ABSOLUTE ${ARG_TEMPLATE_FILE})
        set(TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_TEMPLATE_FILE})
    else()
        set(TEMPLATE_FILE ${ARG_TEMPLATE_FILE})
    endif()

    _hypertextcppImpl_StringAfterLast(${TEMPLATE_FILE} / TEMPLATE_NAME)
    _hypertextcppImpl_StringBeforeLast(${TEMPLATE_NAME} "." TEMPLATE_NAME)


    if (ARG_OUTPUT_DIR)
        if (NOT IS_ABSOLUTE ${ARG_OUTPUT_DIR})
            set(OUTPUT_HEADER_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.h)
            set(OUTPUT_SOURCE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.cpp)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${CMAKE_CURRENT_SOURCE_DIR}/${ARG_OUTPUT_DIR}")
        else()
            set(OUTPUT_HEADER_FILE ${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.h)
            set(OUTPUT_SOURCE_FILE ${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.cpp)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${ARG_OUTPUT_DIR}")
        endif()
    else()
        _hypertextcppImpl_StringBeforeLast(${TEMPLATE_FILE} / TEMPLATE_FILE_DIR)
        set(OUTPUT_HEADER_FILE ${TEMPLATE_FILE_DIR}/${TEMPLATE_NAME}.h)
        set(OUTPUT_SOURCE_FILE ${TEMPLATE_FILE_DIR}/${TEMPLATE_NAME}.cpp)
        set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${TEMPLATE_FILE_DIR}")
    endif()

    if (ARG_CLASS_NAME)
        set(CLASS_NAME_CMDLINE_PARAM "-className=${ARG_CLASS_NAME}")
    endif()
    add_custom_command(
            OUTPUT  ${OUTPUT_HEADER_FILE} ${OUTPUT_SOURCE_FILE}
            COMMAND hypertextcpp generateHeaderAndSource ${TEMPLATE_FILE} -configClassName=${ARG_CONFIG_CLASS_NAME} ${OUTPUT_DIR_CMDLINE_PARAM} ${CLASS_NAME_CMDLINE_PARAM}
            DEPENDS ${TEMPLATE_FILE}
            VERBATIM
    )
endfunction()

function(hypertextcpp_BuildSharedLibrary)
    cmake_parse_arguments(
            ARG
            ""
            "TEMPLATE_FILE;OUTPUT_DIR"
            ""
            ${ARGN}
    )
    if (NOT ARG_TEMPLATE_FILE)
        message(FATAL_ERROR "[hypertextcpp_BuildSharedLibrary] Argument 'TEMPLATE_FILE' is missing")
    endif()
    if (ARG_UNPARSED_ARGUMENTS)
        message(FATAL_ERROR "[hypertextcpp_BuildSharedLibrary] Unsupported argument: ${ARG_UNPARSED_ARGUMENTS}")
    endif()

    if (NOT IS_ABSOLUTE ${ARG_TEMPLATE_FILE})
        set(TEMPLATE_FILE ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_TEMPLATE_FILE})
    else()
        set(TEMPLATE_FILE ${ARG_TEMPLATE_FILE})
    endif()

    _hypertextcppImpl_StringAfterLast(${TEMPLATE_FILE} / TEMPLATE_NAME)
    _hypertextcppImpl_StringBeforeLast(${TEMPLATE_NAME} "." TEMPLATE_NAME)

    if (ARG_OUTPUT_DIR)
        if (NOT IS_ABSOLUTE ${ARG_OUTPUT_DIR})
            set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.cpp)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${CMAKE_CURRENT_BINARY_DIR}/${ARG_OUTPUT_DIR}")
        else()
            set(OUTPUT_FILE ${ARG_OUTPUT_DIR}/${TEMPLATE_NAME}.cpp)
            set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${ARG_OUTPUT_DIR}")
        endif()
    else()
        set(OUTPUT_FILE ${CMAKE_CURRENT_BINARY_DIR}/${TEMPLATE_NAME}.cpp)
        set(OUTPUT_DIR_CMDLINE_PARAM "-outputDir=${CMAKE_CURRENT_BINARY_DIR}")
    endif()

    add_custom_command(
            OUTPUT  ${OUTPUT_FILE}
            COMMAND hypertextcpp generateSharedLibrarySource ${TEMPLATE_FILE} ${OUTPUT_DIR_CMDLINE_PARAM}
            DEPENDS ${TEMPLATE_FILE}
            VERBATIM
    )

    add_library(${TEMPLATE_NAME} SHARED ${OUTPUT_FILE})
    target_compile_features(${TEMPLATE_NAME} PUBLIC cxx_std_11)
    target_compile_definitions(${TEMPLATE_NAME} PUBLIC "HYPERTEXTCPP_EXPORT")
    set_target_properties(${TEMPLATE_NAME} PROPERTIES CXX_EXTENSIONS OFF)
    if (ARG_OUTPUT_DIR)
        set_target_properties(${TEMPLATE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ARG_OUTPUT_DIR})
    endif()
endfunction()

function (_hypertextcppImpl_StringBeforeLast STR VALUE RESULT)
    _hypertextcppImpl_StringBefore(${STR} ${VALUE} RESULT_VALUE REVERSE)
    set(${RESULT} ${RESULT_VALUE} PARENT_SCOPE)
endfunction()

function (_hypertextcppImpl_StringAfterLast STR VALUE RESULT)
    _hypertextcppImpl_StringAfter(${STR} ${VALUE} RESULT_VALUE REVERSE)
    set(${RESULT} ${RESULT_VALUE} PARENT_SCOPE)
endfunction()

function(_hypertextcppImpl_StringBefore STR VALUE RESULT REVERSE)
    string(FIND ${STR} ${VALUE} VALUE_POS ${REVERSE})
    string(LENGTH ${STR} STR_LENGTH)
    string(SUBSTRING ${STR}  0 ${VALUE_POS} RESULT_VALUE)
    set(${RESULT} ${RESULT_VALUE} PARENT_SCOPE)
endfunction()

function (_hypertextcppImpl_StringAfter STR VALUE RESULT REVERSE)
    string(FIND ${STR} ${VALUE} VALUE_POS ${REVERSE})
    string(LENGTH ${STR} STR_LENGTH)
    string(LENGTH ${VALUE} VALUE_LENGTH)
    MATH(EXPR RESULT_LENGTH "${STR_LENGTH} - ${VALUE_POS} - ${VALUE_LENGTH}")
    MATH(EXPR RESULT_POS "${VALUE_POS} + ${VALUE_LENGTH}")
    string(SUBSTRING ${STR} ${RESULT_POS} ${RESULT_LENGTH} RESULT_VALUE)
    set(${RESULT} ${RESULT_VALUE} PARENT_SCOPE)
endfunction()
