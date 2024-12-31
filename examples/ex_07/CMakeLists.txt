cmake_minimum_required(VERSION 3.18)
project(ex_07)

include(../../hypertextcpp.cmake)

hypertextcpp_GenerateHeaderAndSource(
        TEMPLATE_FILE todolist.htcpp
        CLASS_NAME TodoList
        CONFIG_CLASS_NAME PageParams)

set(SRC
    todolist_printer.cpp
    todolist.h
    todolist.cpp)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

