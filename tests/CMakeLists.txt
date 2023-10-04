cmake_minimum_required(VERSION 3.18)
project(test_hypertextcpp)

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

SealLake_GoogleTest(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_20
        PROPERTIES
            CXX_EXTENSIONS OFF
        INCLUDES
            ../src
            ${SEAL_LAKE_SOURCE_range-v3}/include
        LIBRARIES
            Microsoft.GSL::GSL
)

