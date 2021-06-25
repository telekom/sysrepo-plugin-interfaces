cmake_minimum_required(VERSION 2.8)
project(sysrepo-plugin-interfaces C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
)

set(SOURCES
    src/interfaces.c
    src/utils/memory.c
    src/utils/if_state.c
    src/utils/link_data.c
    src/utils/ip_data.c
    src/utils/ipv6_data.c
    src/utils/if_nic_stats.c
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
    add_library(${CMAKE_PROJECT_NAME} MODULE ${SOURCES})
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION lib)
else()
    add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
    install(TARGETS ${CMAKE_PROJECT_NAME} DESTINATION bin)
endif()

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} PREFIX "")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)
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
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
    ${NL_INCLUDE_DIRS}
)

if(ENABLE_BUILD_TESTS)
	find_package(CMOCKA REQUIRED)
    include (CTest)
endif()
