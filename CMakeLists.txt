cmake_minimum_required(VERSION 3.18)
project(hypertextcpp VERSION 1.0.0 DESCRIPTION "hypertextcpp")

include(GNUInstallDirs)
include(external/seal_lake)

SealLake_Import(cmdlime 2.5.0
        GIT_REPOSITORY https://github.com/kamchatka-volcano/cmdlime
        GIT_TAG v2.5.0
)

SealLake_Import(gsl 4.0.0
        GIT_REPOSITORY https://github.com/microsoft/GSL.git
        GIT_TAG        v4.0.0
)


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


SealLake_Executable(
        SOURCES ${SRC}
        COMPILE_FEATURES cxx_std_20
        PROPERTIES
            CXX_EXTENSIONS OFF
        INCLUDES ${SEAL_LAKE_SOURCE_range-v3}/include
        LIBRARIES
            cmdlime::cmdlime
            Microsoft.GSL::GSL
)

SealLake_OptionalBuildSteps(tests)


install(DIRECTORY shared_lib_api/include/hypertextcpp
        COMPONENT shared_lib_api
        EXCLUDE_FROM_ALL
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
