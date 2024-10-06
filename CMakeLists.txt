cmake_minimum_required(VERSION 3.18)
project(hypertextcpp VERSION 1.2.1 DESCRIPTION "hypertextcpp")

include(external/seal_lake)

SealLake_Import(cmdlime 2.6.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/cmdlime
        GIT_TAG dev
)

SealLake_Import(sfun 5.1.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/sfun
        GIT_TAG v5.1.0
)

SealLake_Import(gsl 4.0.0
        GIT_REPOSITORY https://github.com/microsoft/GSL.git
        GIT_TAG        v4.0.0
)

SealLake_Bundle(
        NAME range-v3
        SKIP_LOAD
        GIT_REPOSITORY https://github.com/ericniebler/range-v3
        GIT_TAG        0.12.0
)

set(SRC
    src/codenode.cpp
        src/control_flow_statement_node.cpp
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
    src/node_utils.cpp
    src/single_header_transpiler_renderer.cpp
    src/shared_lib_transpiler_renderer.cpp)


SealLake_Executable(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_20
        PROPERTIES
            CXX_EXTENSIONS OFF
        INCLUDES ${SEAL_LAKE_SOURCE_range-v3}/include
        LIBRARIES
            cmdlime::cmdlime
            sfun::sfun
            Microsoft.GSL::GSL
)

SealLake_OptionalSubProjects(tests examples)

install(DIRECTORY shared_lib_api/include/hypertextcpp
        COMPONENT shared_lib_api
        EXCLUDE_FROM_ALL
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
