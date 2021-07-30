cmake_minimum_required(VERSION 3.13)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -fPIC")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

option(NAME "Template name")

add_custom_command(
    OUTPUT  ${NAME}.cpp
    COMMAND hypertextcpp ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp -o ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp -s
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}.htcpp
)

add_library(${NAME} SHARED ${CMAKE_CURRENT_BINARY_DIR}/${NAME}.cpp)