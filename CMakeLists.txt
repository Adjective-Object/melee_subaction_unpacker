cmake_minimum_required (VERSION 3.4)

project (mreader)
set (mreader_VERSION_MAJOR 0)
set (mreader_VERSION_MINOR 0)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

################
# DEPENDENCIES #
################
include(ExternalProject)
ExternalProject_Add(
    libsweetparse
    # no download since it's in a git submodule
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libsweetparse
    CONFIGURE_COMMAND cmake CmakeLists.txt
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)

ExternalProject_Add(
    binscripter
    # no download since it's in a git submodule
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/binscripter
    CONFIGURE_COMMAND cmake CmakeLists.txt
    BUILD_COMMAND make
    BUILD_IN_SOURCE 1
    INSTALL_COMMAND ""
)


# add dependencies to search path
include_directories(AFTER ./src ./libsweetparse/src ./binscripter)
link_directories(AFTER ./libsweetparse ./binscripter)

set(CMAKE_CXX_FLAGS "-fPIC -g -Wall -std=c++11")

##########################
# project source objects #
##########################
set(LIB_SRCS
        src/config.cpp
        src/config.hpp
        src/dolfs.cpp
        src/dolfs.hpp
        src/event_mapper.cpp
        src/event_mapper.hpp
        src/helpers.cpp
        src/helpers.hpp
        src/macros.hpp)
add_library(MREADER_LIB OBJECT ${LIB_SRCS})

set(ALL_SRCS ${LIB_SRCS} src/mreader.cpp)

#####################
# output executable #
#####################
add_executable(mreader
    src/mreader.cpp
    $<TARGET_OBJECTS:MREADER_LIB>)
target_link_libraries(mreader binscript sweetparse)

########################################
# Code formatting and linting commands #
########################################
add_custom_target(lint
    COMMAND cppcheck 
        --std=c11
        --enable=warning
        --enable=missingInclude
        --enable=performance
        -I ./src -I ./libsweetparse/src -I ./binscripter
        src/mreader.cpp ${ALL_SRCS})

add_custom_target(format
    COMMAND clang-format -i ${ALL_SRCS})
    

