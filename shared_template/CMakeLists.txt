cmake_minimum_required(VERSION 3.18)

option(NAME "Template name")

project(${NAME})

add_custom_command(
    OUTPUT  ${NAME}.cpp
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp -o ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp -s
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp
)

add_library(${NAME} SHARED ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp)
target_compile_features(${NAME} PUBLIC cxx_std_17)
set_target_properties(${NAME} PROPERTIES CXX_EXTENSIONS OFF)
