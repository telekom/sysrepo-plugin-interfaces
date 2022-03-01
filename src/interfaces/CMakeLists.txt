cmake_minimum_required(VERSION 2.8)
project(sysrepo-plugin-interfaces C)

set(SOURCES
    interfaces.c
    if_state.c
    link_data.c
    ip_data.c
    ipv6_data.c
    if_nic_stats.c
    datastore.c
    ${CMAKE_SOURCE_DIR}/src/utils/memory.c
)

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

# pthread api
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

target_link_libraries(
    ${PROJECT_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${NL_LIBRARIES}
    Threads::Threads
)

include_directories(
    ${NL_INCLUDE_DIRS}
)
