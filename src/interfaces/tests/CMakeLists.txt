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
add_library(
   ${UTILS_MEMORY_LIBRARY_NAME} 
   STATIC
   ${CMAKE_SOURCE_DIR}/src/utils/memory.c
)
target_link_options(
    ${INTERFACES_UTEST_NAME}
    PRIVATE ${INTERFACES_UTEST_LINKER_OPTIONS}
)
target_link_libraries(
    ${INTERFACES_UTEST_NAME}

	${CMOCKA_LIBRARIES}
    ${PLUGIN_LIBRARY_NAME}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
    ${UTILS_MEMORY_LIBRARY_NAME}
)
add_test(
    NAME ${INTERFACES_UTEST_NAME}
    COMMAND interfaces_utest
)
