#
# telekom / sysrepo-plugin-interfaces
#
# This program is made available under the terms of the
# BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2021 Deutsche Telekom AG
# SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#
cmake_minimum_required(VERSION 3.0)
project(ietf-interfaces-plugin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(PLUGIN_LIRBARY_NAME srplg-ietf-interfaces)
set(PLUGIN_EXECUTABLE_NAME ietf-interfaces-plugin)
set(PLUGIN_CORE_LIBRARY_NAME ietf-interfaces-plugin-core)

find_package(PkgConfig REQUIRED)
find_package(SYSREPOCPP REQUIRED)
find_package(LIBYANGCPP REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPCPP REQUIRED)
find_package(NL REQUIRED)

include_directories(
    ${SYSREPOCPP_INCLUDE_DIRS}
    ${LIBYANGCPP_INCLUDE_DIRS}
    ${SRPCPP_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/src/interfaces/src
)

set(
    CORE_SOURCES

    src/core/context.cpp
)

set(
    MODULE_SOURCES

    src/modules/interfaces.cpp
    src/modules/interfaces/oper.cpp
    src/modules/interfaces/context.cpp
    src/modules/interfaces/common.cpp
    src/modules/interfaces/api/nl.cpp
    src/modules/interfaces/api/interface.cpp
    src/modules/interfaces/api/address.cpp
    src/modules/interfaces/api/neighbor.cpp
)

set(
    PLUGIN_SOURCES

    src/plugin.cpp
)

set(
    EXECUTABLE_SOURCES

    src/main.c
    src/plugin.cpp
)

# core library
add_library(
    ${PLUGIN_CORE_LIBRARY_NAME}
    STATIC
    ${CORE_SOURCES}
    ${MODULE_SOURCES}
)

target_compile_options(
    ${PLUGIN_CORE_LIBRARY_NAME}
    PRIVATE
    -fPIC
)

# plugin as an executable
add_executable(
    ${PLUGIN_EXECUTABLE_NAME}
    ${EXECUTABLE_SOURCES}
)

target_link_libraries(
    ${PLUGIN_EXECUTABLE_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SYSREPOCPP_LIBRARIES}
    ${LIBYANGCPP_LIBRARIES}
    ${SRPCPP_LIBRARIES}
    ${NL_LIBRARIES}
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
    ${SRPCPP_LIBRARIES}
    ${NL_LIBRARIES}
)
