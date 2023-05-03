cmake_minimum_required(VERSION 3.0)
project(ietf-interfaces-plugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(PLUGIN_LIRBARY_NAME ietf-interfaces-plugin)

find_package(SYSREPOCPP REQUIRED)
find_package(LIBYANGCPP REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)

include_directories(
    ${SYSREPOCPP_INCLUDE_DIRS}
    ${LIBYANGCPP_INCLUDE_DIRS}
    ./src
)

set(
    PLUGIN_HEADERS

    src/core/context.cpp
    src/core/sub/change.cpp
    src/core/sub/oper.cpp
    src/core/sub/rpc.cpp
    src/core/yang/tree.cpp
    src/plugin.cpp
)


set(
    PLUGIN_SOURCES

    src/core/context.cpp
    src/core/sub/change.cpp
    src/core/sub/oper.cpp
    src/core/sub/rpc.cpp
    src/core/yang/tree.cpp
    src/plugin.cpp
)

add_library(
    ${PLUGIN_LIRBARY_NAME}
    MODULE
    ${PLUGIN_SOURCES}
)

target_link_libraries(
    ${PLUGIN_LIRBARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
)