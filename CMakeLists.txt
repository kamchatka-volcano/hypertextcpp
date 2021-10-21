cmake_minimum_required(VERSION 3.18)
project(hypertextcpp VERSION 1.0.0 DESCRIPTION "hypertextcpp")

include(external/cmdlime)
include(external/gsl)

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
    src/utils.cpp
    src/singleheadertranspiler.cpp
    src/sharedlibtranspiler.cpp)
    
add_executable(hypertextcpp ${SRC})
target_compile_features(hypertextcpp PUBLIC cxx_std_17)
set_target_properties(hypertextcpp PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(hypertextcpp PRIVATE cmdlime GSL)

option(ENABLE_TESTS "Enable tests" OFF)
if (ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(TARGETS hypertextcpp RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(DIRECTORY shared_lib_api/include/hypertextcpp
        COMPONENT shared_lib_api
        EXCLUDE_FROM_ALL
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
