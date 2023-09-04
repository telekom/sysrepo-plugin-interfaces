#
# telekom / sysrepo-plugin-system
#
# This program is made available under the terms of the BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2022 Deutsche Telekom AG SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#
cmake_minimum_required(VERSION 3.0)
project(sysrepo-interfaces-plugins C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SRPC REQUIRED)
find_package(NL REQUIRED)

# get sysrepo version
find_package(PkgConfig)
if(PKG_CONFIG_FOUND)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "--modversion" "sysrepo" OUTPUT_VARIABLE SYSREPO_VERSION)
    if(SYSREPO_VERSION)
        # strip new line from string
        string(STRIP ${SYSREPO_VERSION} SYSREPO_VERSION)
        if(${SYSREPO_VERSION} VERSION_LESS "1.0.0")
            message(FATAL_ERROR "${PROJECT_NAME} requires at least libsysrepo verision 1.0.0")
        endif()
    endif()
endif()

option(INTERFACES_PLUGIN "Enable interfaces plugin" OFF)
option(ROUTING_PLUGIN "Enable routing plugin" OFF)
option(BRIDGING_PLUGIN "Enable bridging plugin" OFF)

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
