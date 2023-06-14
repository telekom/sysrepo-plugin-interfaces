#
# telekom / sysrepo-plugin-system
#
# This program is made available under the terms of the
# BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
# SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#
cmake_minimum_required(VERSION 3.0)
project(sysrepo-interfaces-plugins) 

set(CMAKE_CXX_STANDARD 20)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")

# include_directories(
#     ${CMAKE_SOURCE_DIR}/src/
#     ${CMAKE_SOURCE_DIR}/deps/uthash/include/
# )

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(PLUGIN_LIRBARY_NAME srplg-ietf-interfaces)
set(PLUGIN_EXECUTABLE_NAME ietf-interfaces-plugin-test)
set(PLUGIN_CORE_LIBRARY_NAME ietf-interfaces-plugin-core)

find_package(SYSREPOCPP REQUIRED)
find_package(LIBYANGCPP REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(NL REQUIRED)

include_directories(
    ${SYSREPOCPP_INCLUDE_DIRS}
    ${LIBYANGCPP_INCLUDE_DIRS}
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src
)

set (
    CORE_HEADERS

    src/interfaces/src/core/context.hpp
    src/interfaces/src/core/sub/change.hpp
    src/interfaces/src/core/sub/oper.hpp
    src/interfaces/src/core/sub/rpc.hpp
    src/interfaces/src/core/yang/tree.hpp

    src/interfaces/src/core/system/interface.hpp
    src/interfaces/src/core/system/IPV4.hpp
    src/interfaces/src/core/system/Neighbour.hpp
    src/interfaces/src/core/system/Address.hpp
)

set (
    CORE_SOURCES

    src/interfaces/src/core/context.cpp
    src/interfaces/src/core/sub/change.cpp
    src/interfaces/src/core/sub/oper.cpp
    src/interfaces/src/core/sub/rpc.cpp
    src/interfaces/src/core/yang/tree.cpp

    src/interfaces/src/core/system/interface.cpp
    src/interfaces/src/core/system/IPV4.cpp
    src/interfaces/src/core/system/Neighbour.cpp
    src/interfaces/src/core/system/Address.cpp
)

set(
    PLUGIN_HEADERS

    src/interfaces/src/plugin.hpp
)

set(
    PLUGIN_SOURCES

    src/interfaces/src/plugin.cpp
)

set(
    EXECUTABLE_SOURCES

    src/interfaces/src/main.c
    src/interfaces/src/plugin.cpp
)

add_library(
    ${PLUGIN_CORE_LIBRARY_NAME}
    STATIC
    ${CORE_SOURCES}
)

target_compile_options(
    ${PLUGIN_CORE_LIBRARY_NAME}
    PRIVATE
    -fPIC
)

add_executable(
    ${PLUGIN_EXECUTABLE_NAME}
    ${EXECUTABLE_SOURCES}
)

target_link_libraries(
    ${PLUGIN_EXECUTABLE_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}
    ${NL_LIBRARIES}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
   
)

# plugin as a library
add_library(
    ${PLUGIN_LIRBARY_NAME}
    MODULE
    ${PLUGIN_SOURCES}
)

target_link_libraries(
    ${PLUGIN_LIRBARY_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
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
