cmake_minimum_required(VERSION 3.18)
 
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include(CheckPIESupported)
check_pie_supported()

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
target_compile_features(test_hypertextcpp PUBLIC cxx_std_17)
set_target_properties(test_hypertextcpp PROPERTIES CXX_EXTENSIONS OFF POSITION_INDEPENDENT_CODE OFF)
add_test(NAME test_hypertextcpp COMMAND test_hypertextcpp)
target_include_directories(test_hypertextcpp PRIVATE ../src)
target_include_directories(test_hypertextcpp PRIVATE ../lib/gsl/include)
target_link_libraries(test_hypertextcpp PRIVATE ${GTEST_BOTH_LIBRARIES} Threads::Threads)
