cmake_minimum_required(VERSION 3.18)

add_custom_command(
    OUTPUT  todolist.h
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp -o ${CMAKE_CURRENT_SOURCE_DIR}/todolist.h --class-name TodoList
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp
)

set(SRC
    todolist_printer.cpp
    todolist.h)

add_executable(todolist_printer ${SRC})

target_compile_features(todolist_printer PUBLIC cxx_std_17)
set_target_properties(todolist_printer PROPERTIES CXX_EXTENSIONS OFF)

