cmake_minimum_required(VERSION 2.8)
project(sysrepo-plugin-routing C)

include_directories(
	${PROJECT_SOURCE_DIR}
)

set(
    SOURCES
    routing.c
    ${CMAKE_SOURCE_DIR}/src/utils/memory.c
    rib.c
    rib/list.c
    route/list.c
    route/list_hash.c
    route/next_hop.c
    route.c
    control_plane_protocol.c
    common.c

    # subscriptions
    subscription/change.c
    subscription/rpc.c
    subscription/operational.c
)

if (NOT PLUGIN)
	set(SOURCES ${SOURCES} "main.c")
endif()

# get sysrepo version
find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} "--modversion" "sysrepo" OUTPUT_VARIABLE SYSREPO_VERSION)
    if(SYSREPO_VERSION)
        # strip new line from string
        string(STRIP ${SYSREPO_VERSION} SYSREPO_VERSION)
        if(${SYSREPO_VERSION} VERSION_LESS "1.0.0")
            message(FATAL_ERROR "${PROJECT_NAME} requires at least libsysrepo verision 1.0.0")
        endif()
    endif()
endif()

if(PLUGIN)
    add_library(${PROJECT_NAME} MODULE ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION lib)
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME} PREFIX "")

find_package(NL REQUIRED)

target_link_libraries(
    ${PROJECT_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${NL_LIBRARIES}
)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
)
