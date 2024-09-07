cmake_minimum_required(VERSION 3.18)
project(ex_04)

add_custom_command(
    OUTPUT  todolist.h
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp -o ${CMAKE_CURRENT_SOURCE_DIR}/todolist.h --class-name TodoList
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp
    VERBATIM
)

set(SRC
    todolist_printer.cpp
    todolist.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

