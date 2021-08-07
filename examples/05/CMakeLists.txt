cmake_minimum_required(VERSION 3.18)

set(SRC
    todolist_printer.cpp)
    
add_executable(todolist_printer ${SRC})

target_compile_features(todolist_printer PUBLIC cxx_std_17)
set_target_properties(todolist_printer PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(todolist_printer ${CMAKE_DL_LIBS})
