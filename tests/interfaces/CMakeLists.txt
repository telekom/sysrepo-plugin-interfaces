set(INTERFACES_UTEST_NAME "interfaces_utest")

add_executable(
    ${INTERFACES_UTEST_NAME}

    "interfaces_utest.c"
)
target_include_directories(
    ${INTERFACES_UTEST_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/interfaces/src
)
target_link_libraries(
    ${INTERFACES_UTEST_NAME}

	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
)
add_test(
    NAME ${INTERFACES_UTEST_NAME}
    COMMAND interfaces_utest
)
