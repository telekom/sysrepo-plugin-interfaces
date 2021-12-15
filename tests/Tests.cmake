
# routing_utest
add_executable(
    "routing_utest"

    "tests/routing/routing_utest.c"
)
target_link_libraries(
    "routing_utest"

	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
)
add_test(
    NAME routing_utest
    COMMAND routing_utest
)