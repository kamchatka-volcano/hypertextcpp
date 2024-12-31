cmake_minimum_required(VERSION 3.18)
project(ex_05)

set(SRC
    todolist_printer.cpp)
    
add_executable(${PROJECT_NAME} ${SRC})

target_include_directories(${PROJECT_NAME} PUBLIC ../../shared_lib_api/include)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

include(../../hypertextcpp.cmake)
hypertextcpp_BuildSharedLibrary(
        TEMPLATE_FILE todolist.htcpp
)
add_dependencies(${PROJECT_NAME} todolist)