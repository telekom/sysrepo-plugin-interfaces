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
project(sysrepo-plugin-interfaces C)

set(PLUGIN_CORE_LIBRARY_NAME "srplg-ietf-interfaces-core")
set(PLUGIN_MODULE_NAME "srplg-ietf-interfaces")
set(PLUGIN_EXECUTABLE_NAME "ietf-interfaces-plugin")

include_directories(
    ${CMAKE_SOURCE_DIR}/src/interfaces/src
    ${CMAKE_SOURCE_DIR}/deps/uthash
    ${CMAKE_SOURCE_DIR}/src/utils
)

# set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(
    CORE_SOURCES

    # subscription
    src/plugin/subscription/change.c
    src/plugin/subscription/operational.c
    src/plugin/subscription/rpc.c

    # ly_tree
    src/plugin/ly_tree.c
    src/plugin/common.c

    # API
    src/plugin/api/interfaces/check.c
    src/plugin/api/interfaces/load.c
    src/plugin/api/interfaces/read.c
    src/plugin/api/interfaces/store.c
    src/plugin/api/interfaces/change.c
    src/plugin/api/interfaces/interface/load.c
    src/plugin/api/interfaces/interface/change.c
    src/plugin/api/interfaces/interface/ipv6/load.c
    src/plugin/api/interfaces/interface/ipv6/change.c
    src/plugin/api/interfaces/interface/ipv6/autoconf/change.c
    src/plugin/api/interfaces/interface/ipv6/neighbor/change.c
    src/plugin/api/interfaces/interface/ipv6/neighbor/load.c
    src/plugin/api/interfaces/interface/ipv6/address/change.c
    src/plugin/api/interfaces/interface/ipv6/address/load.c
    src/plugin/api/interfaces/interface/ipv4/change.c
    src/plugin/api/interfaces/interface/ipv4/load.c
    src/plugin/api/interfaces/interface/ipv4/neighbor/change.c
    src/plugin/api/interfaces/interface/ipv4/neighbor/load.c
    src/plugin/api/interfaces/interface/ipv4/address/change.c
    src/plugin/api/interfaces/interface/ipv4/address/load.c
    src/plugin/api/interfaces/interface/encapsulation/dot1q-vlan/second-tag/change.c
    src/plugin/api/interfaces/interface/encapsulation/dot1q-vlan/outer-tag/change.c
    src/plugin/api/interfaces/interface/dampening/change.c
    src/plugin/api/interfaces/interface/carrier-delay/change.c

    # data
    src/plugin/data/interfaces/interface.c
    src/plugin/data/interfaces/interface_state.c
    src/plugin/data/interfaces/interface/ipv4.c
    src/plugin/data/interfaces/interface/ipv6.c
    src/plugin/data/interfaces/interface/ipv4/address.c
    src/plugin/data/interfaces/interface/ipv4/neighbor.c
    src/plugin/data/interfaces/interface/ipv6/address.c
    src/plugin/data/interfaces/interface/ipv6/neighbor.c

    # main files
    ${CMAKE_SOURCE_DIR}/src/utils/memory.c
)

set(
    PLUGIN_SOURCES

    src/plugin.c

    # startup
    src/plugin/startup/load.c
    src/plugin/startup/store.c

    # running
    src/plugin/running/load.c
    src/plugin/running/store.c
)

# packages
find_package(AUGYANG)
find_package(PTHREAD REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(SRPC REQUIRED)
find_package(NL REQUIRED)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
    ${PTHREAD_INCLUDE_DIRS}
)

# build core library
add_library(${PLUGIN_CORE_LIBRARY_NAME} STATIC ${CORE_SOURCES})
target_compile_options(
    ${PLUGIN_CORE_LIBRARY_NAME}
    PRIVATE
    -fPIC
)

# build plugin as a module
add_library(
    ${PLUGIN_MODULE_NAME}
    MODULE
    ${PLUGIN_SOURCES}
    # ${CORE_SOURCES}
)
target_link_libraries(
    ${PLUGIN_MODULE_NAME}

    ${PLUGIN_CORE_LIBRARY_NAME}

    # link external dependencies
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${NL_LIBRARIES}
    ${PTHREAD_LIBRARIES}
)

# build plugin as a standalone executable
add_executable(
    ${PLUGIN_EXECUTABLE_NAME}
    ${PLUGIN_SOURCES}

    # add main
    src/main.c
)
target_link_libraries(
    ${PLUGIN_EXECUTABLE_NAME}

    # link core lib
    ${PLUGIN_CORE_LIBRARY_NAME}

    # link external dependencies
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${NL_LIBRARIES}
    ${PTHREAD_LIBRARIES}
)

# install to appropriate directories
install(TARGETS ${PLUGIN_MODULE_NAME} DESTINATION lib)
install(TARGETS ${PLUGIN_EXECUTABLE_NAME} DESTINATION bin)

# augyang support
if(AUGYANG_FOUND)
    add_compile_definitions(AUGYANG)
else(AUGYANG_FOUND)
    message(WARNING "AUGYANG not found - augeas support will be disabled")
endif()

if(ENABLE_BUILD_TESTS)
    find_package(CMOCKA REQUIRED)
    include(CTest)
    include(tests/CMakeLists.txt)
endif()
