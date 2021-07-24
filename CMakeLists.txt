cmake_minimum_required(VERSION 3.13)

project(hypertextcpp VERSION 1.0.0 DESCRIPTION "hypertextcpp")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(SRC
    src/codenode.cpp
    src/main.cpp
    src/procedurenode.cpp
    src/nameutils.cpp
    src/nodeextension.cpp
    src/nodereader.cpp
    src/sectionnode.cpp
    src/streamreader.cpp    
    src/tagnode.cpp
    src/textnode.cpp
    src/transpiler.cpp
    src/utils.cpp)
    
add_executable(hypertextcpp ${SRC})
target_include_directories(hypertextcpp PRIVATE lib/cmdlime/include)
target_include_directories(hypertextcpp PRIVATE lib/gsl/include)
target_link_libraries(hypertextcpp stdc++fs)

option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(TARGETS hypertextcpp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY shared_lib_api/include/hypertextcpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
