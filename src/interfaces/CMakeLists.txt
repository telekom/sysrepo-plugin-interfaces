cmake_minimum_required(VERSION 3.0)
project(sysrepo-plugin-interfaces C)

set(PLUGIN_LIBRARY_NAME srplg-interfaces)

include_directories(
    ${CMAKE_SOURCE_DIR}/src/interfaces/src
    ${CMAKE_SOURCE_DIR}/deps/uthash
)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")

set(
    SOURCES

    src/plugin/startup/load.c
    src/plugin/startup/store.c
    src/plugin/subscription/change.c
    src/plugin/subscription/operational.c
    src/plugin/subscription/rpc.c
    src/plugin/ly_tree.c
    src/plugin/api/interfaces/check.c
    src/plugin/api/interfaces/load.c
    src/plugin/api/interfaces/store.c
    src/plugin/api/interfaces/change.c
    src/plugin/api/interfaces/interface/change.c
    src/plugin/api/interfaces/interface/encapsulation/dot1q-vlan/second-tag/change.c
    src/plugin/api/interfaces/interface/encapsulation/dot1q-vlan/outer-tag/change.c
    src/plugin/api/interfaces/interface/dampening/change.c
    src/plugin/api/interfaces/interface/carrier-delay/change.c
    src/plugin/data/interfaces/interface/state.c
    src/plugin.c
)

# packages
find_package(AUGYANG)
find_package(PTHREAD REQUIRED)
find_package(LIBYANG REQUIRED)
find_package(SYSREPO REQUIRED)
find_package(SRPC REQUIRED)
find_package(NL REQUIRED)

# plugin library
add_library(
    ${PLUGIN_LIBRARY_NAME}
    STATIC
    ${SOURCES}
)

# link plugin library
target_link_libraries(
    ${PLUGIN_LIBRARY_NAME}

    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES}
    ${SRPC_LIBRARIES}
    ${NL_LIBRARIES}
    ${PTHREAD_LIBRARIES}
)

if(PLUGIN)
    # ignore plugin library and compile PROJECT_NAME as a module
    add_library(
        ${PROJECT_NAME}
        MODULE ${SOURCES}
    )
    target_link_libraries(
        ${PROJECT_NAME}
        ${SYSREPO_LIBRARIES}
        ${LIBYANG_LIBRARIES}
        ${SRPC_LIBRARIES}
        ${NL_LIBRARIES}
        ${PTHREAD_LIBRARIES}
    )
else()
    add_executable(
        ${PROJECT_NAME}
        src/main.c
    )
    target_link_libraries(
        ${PROJECT_NAME}

        # link plugin library with executable
        ${PLUGIN_LIBRARY_NAME}

        ${SYSREPO_LIBRARIES}
        ${LIBYANG_LIBRARIES}
        ${SRPC_LIBRARIES}
        ${NL_LIBRARIES}
        ${PTHREAD_LIBRARIES}
    )
endif()

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${SRPC_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
    ${PTHREAD_INCLUDE_DIRS}
)

# augyang support
if(AUGYANG_FOUND)
    add_compile_definitions(AUGYANG)
else(AUGYANG_FOUND)
    message(WARNING "AUGYANG not found - augeas support will be disabled")
endif()