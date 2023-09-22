set(ROUTING_UTEST_NAME "routing_utest")
set(BRIDGING_UTEST_NAME "bridging_utest")

# routing_utest
add_executable(
    ${ROUTING_UTEST_NAME}
    "tests/routing/routing_utest.c"
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

# bridging_utest
add_executable(
    ${BRIDGING_UTEST_NAME}
    "tests/bridging/bridging_utest.c"
    ${CMAKE_SOURCE_DIR}/src/bridging/plugin/common.c
)
target_link_libraries(
    ${BRIDGING_UTEST_NAME}
	${CMOCKA_LIBRARIES}
	${SYSREPO_LIBRARIES}
	${LIBYANG_LIBRARIES}
)
target_include_directories(
    ${BRIDGING_UTEST_NAME}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/
    PUBLIC ${CMAKE_SOURCE_DIR}/src/utils
    PUBLIC ${CMAKE_SOURCE_DIR}/src/bridging
    PUBLIC ${CMAKE_SOURCE_DIR}/deps)
add_test(
    NAME ${BRIDGING_UTEST_NAME}
    COMMAND bridging_utest
)
