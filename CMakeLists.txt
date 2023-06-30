cmake_minimum_required(VERSION 2.8)
project(sysrepo-interfaces-plugins C)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

include_directories(
    ${CMAKE_SOURCE_DIR}/src/
    ${CMAKE_SOURCE_DIR}/deps/uthash/include/
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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")
find_package(SYSREPO REQUIRED)
find_package(LIBYANG REQUIRED)

include_directories(
    ${SYSREPO_INCLUDE_DIRS}
    ${LIBYANG_INCLUDE_DIRS}
)

option(INTERFACES_PLUGIN "Enable interfaces plugin" ON)
option(ROUTING_PLUGIN "Enable interfaces plugin" ON)

if(INTERFACES_PLUGIN)
	add_subdirectory(src/interfaces)
endif()

if(ROUTING_PLUGIN)
	add_subdirectory(src/routing)
endif()

if(BRIDGING_PLUGIN)
	add_subdirectory(src/bridging)
endif()

if(ENABLE_BUILD_TESTS)
	find_package(CMOCKA REQUIRED)
    include (CTest)
    include(tests/Tests.cmake)
endif()
