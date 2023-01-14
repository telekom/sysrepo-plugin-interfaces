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
set(INTERFACES_UTEST_NAME "interfaces_utest")
set(UTILS_MEMORY_LIBRARY_NAME  "utils_memory")

# for wrapping cmocka mock functions
set(
    INTERFACES_UTEST_LINKER_OPTIONS
    #"-Wl,--wrap=function_foo"
)

add_executable(
    ${INTERFACES_UTEST_NAME}

    ${CMAKE_SOURCE_DIR}/src/interfaces/tests/interfaces_utest.c
)

target_include_directories(
    ${INTERFACES_UTEST_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/interfaces/src
)

target_link_options(
    ${INTERFACES_UTEST_NAME}
    PRIVATE ${INTERFACES_UTEST_LINKER_OPTIONS}
)

target_link_libraries(
    ${INTERFACES_UTEST_NAME}

	${CMOCKA_LIBRARIES}

    ${PLUGIN_CORE_LIBRARY_NAME}
    
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${NL_LIBRARIES}
    ${PTHREAD_LIBRARIES}
)

add_test(
    NAME ${INTERFACES_UTEST_NAME}
    COMMAND interfaces_utest
)
