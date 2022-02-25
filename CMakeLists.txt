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
project(sysrepo-interfaces-plugins C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")

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

if(BRIDGING_PLUGIN)
	add_subdirectory(src/bridging)
endif()

if(ENABLE_BUILD_TESTS)
    find_package(CMOCKA REQUIRED)
    include(CTest)
    include(tests/Tests.cmake)
endif()
