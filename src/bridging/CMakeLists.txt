#
# telekom / sysrepo-plugin-interfaces
#
# This program is made available under the terms of the
# BSD 3-Clause license which is available at
# https://opensource.org/licenses/BSD-3-Clause
#
# SPDX-FileCopyrightText: 2023 Deutsche Telekom AG
# SPDX-FileContributor: Sartura Ltd.
#
# SPDX-License-Identifier: BSD-3-Clause
#

project(sysrepo-plugin-bridging C)

# set(PLUGIN_CORE_LIBRARY_NAME "srplg-bridging-core")
# set(PLUGIN_MODULE_NAME "srplg-bridging")
# set(PLUGIN_EXECUTABLE_NAME "bridging-plugin")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(NL REQUIRED)

set(SOURCES
    plugin/api/bridge_netlink.c
    plugin/data/component/list.c
    plugin/data/vlan/list.c
    plugin/data/bridge.c
    plugin/data/component.c
    plugin/data/list.c
    plugin/startup/load.c
    plugin/subscription/change.c
    plugin/subscription/operational.c
    plugin/common.c
    plugin/ly_tree.c
    plugin.c

    # utils
    ${CMAKE_SOURCE_DIR}/src/utils/memory.c)

if(PLUGIN)
    add_library(${PROJECT_NAME} MODULE ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION lib)
else()
    set(SOURCES ${SOURCES} main.c)
    add_executable(${PROJECT_NAME} ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()

# set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} PREFIX "")

target_link_libraries(
    ${PROJECT_NAME}
    ${SRPC_LIBRARIES}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${NL_LIBRARIES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${SYSREPO_INCLUDE_DIRS}
    PUBLIC ${SRPC_INCLUDE_DIRS}
    PUBLIC ${LIBYANG_INCLUDE_DIRS}
    PUBLIC ${NL_INCLUDE_DIRS}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/
    PUBLIC ${CMAKE_SOURCE_DIR}/src/bridging
    PUBLIC ${CMAKE_SOURCE_DIR}/src/utils
    PRIVATE ${CMAKE_SOURCE_DIR}/deps)
