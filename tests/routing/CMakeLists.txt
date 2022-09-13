set(ROUTING_UTEST_NAME "routing_utest")

add_executable(
    ${ROUTING_UTEST_NAME}

    "routing_utest.c"
)
target_include_directories(
    ${ROUTING_UTEST_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/routing/src
)
target_link_libraries(
    ${ROUTING_UTEST_NAME}

	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
)
add_test(
    NAME ${ROUTING_UTEST_NAME}
    COMMAND routing_utest
)
