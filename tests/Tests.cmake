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

# bridging_utest
add_executable(
    "bridging_utest"

    "tests/bridging/bridging_utest.c"
)
target_link_libraries(
    "bridging_utest"

	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
)
add_test(
    NAME bridging_utest
    COMMAND bridging_utest
)
