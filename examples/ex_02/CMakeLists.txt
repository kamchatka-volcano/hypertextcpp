cmake_minimum_required(VERSION 3.18)
project(ex_02)

include(../../hypertextcpp.cmake)

hypertextcpp_GenerateHeader(
        TEMPLATE_FILE todolist.htcpp
        OUTPUT_DIR template
)

set(SRC
        todolist_printer.cpp
        template/todolist.h)

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_11)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

