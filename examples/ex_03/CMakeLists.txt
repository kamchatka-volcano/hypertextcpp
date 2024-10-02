cmake_minimum_required(VERSION 3.18)
project(ex_03)

include(../../hypertextcpp.cmake)

hypertextcpp_GenerateHeader(NAME todolist)

set(SRC
        todolist_printer.cpp
        todolist.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

