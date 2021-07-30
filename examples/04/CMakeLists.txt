cmake_minimum_required(VERSION 3.13)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_custom_command(
    OUTPUT  todolist.h
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp -o ${CMAKE_CURRENT_SOURCE_DIR}/todolist.h --class-name TodoList
   DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/todolist.htcpp
)

set(SRC
    todolist_printer.cpp
    todolist.h)

add_executable(todolist_printer ${SRC})
