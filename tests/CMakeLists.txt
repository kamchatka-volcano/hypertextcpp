cmake_minimum_required(VERSION 3.13)
 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS}, ..)

set(SRC
    test_codenode.cpp
    test_procedurenode.cpp
    test_nameutils.cpp
    test_sectionnode.cpp
    test_tagnode.cpp
    test_utils.cpp
    ../src/codenode.cpp
    ../src/procedurenode.cpp
    ../src/nameutils.cpp
    ../src/nodeextension.cpp
    ../src/nodereader.cpp
    ../src/sectionnode.cpp
    ../src/streamreader.cpp    
    ../src/tagnode.cpp
    ../src/textnode.cpp
    ../src/transpiler.cpp
    ../src/utils.cpp
    )

add_executable(test_hypertextcpp ${SRC})
add_test(NAME test_hypertextcpp COMMAND test_hypertextcpp)
target_include_directories(test_hypertextcpp PRIVATE ../src)
target_include_directories(test_hypertextcpp PRIVATE ../lib/gsl/include)
target_link_libraries(test_hypertextcpp gtest gmock gtest_main pthread)
