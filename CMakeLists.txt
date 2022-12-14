cmake_minimum_required(VERSION 3.0)
project(sysrepo-interfaces-plugins C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include/
)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(NL REQUIRED)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
)

option(INTERFACES_PLUGIN "Enable interfaces plugin" OFF)
option(ROUTING_PLUGIN "Enable interfaces plugin" OFF)

if(INTERFACES_PLUGIN)
    add_subdirectory(src/interfaces)
endif()

if(ROUTING_PLUGIN)
    add_subdirectory(src/routing)
endif()

if(ENABLE_BUILD_TESTS)
    find_package(CMOCKA REQUIRED)
    include(CTest)
    include(tests/Tests.cmake)
endif()
