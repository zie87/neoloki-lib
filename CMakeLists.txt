cmake_minimum_required(VERSION 3.10)

set(WOTH_CMAKE_MODULES_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(APPEND CMAKE_MODULE_PATH ${WOTH_CMAKE_MODULES_PATH})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(loki-neo LANGUAGES C CXX)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
    set(CMAKE_BUILD_TYPE
        RelWithDebInfo
        CACHE STRING "Choose the type of build." FORCE
    )

    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()


add_library(lokineo INTERFACE)
target_include_directories(lokineo INTERFACE include)
add_library(lokineo::loki ALIAS lokineo)

add_subdirectory(tests)
