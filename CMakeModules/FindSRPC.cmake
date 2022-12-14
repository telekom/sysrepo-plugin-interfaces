if (SRPC_LIBRARIES AND SRPC_INCLUDE_DIRS)
    set(SRPC_FOUND TRUE)
else ()

    find_path(
        SRPC_INCLUDE_DIR
        NAMES srpc.h
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        SRPC_LIBRARY
        NAMES srpc
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if (SRPC_INCLUDE_DIR AND SRPC_LIBRARY)
        set(SRPC_FOUND TRUE)
    else (SRPC_INCLUDE_DIR AND SRPC_LIBRARY)
        set(SRPC_FOUND FALSE)
    endif (SRPC_INCLUDE_DIR AND SRPC_LIBRARY)

    set(SRPC_INCLUDE_DIRS ${SRPC_INCLUDE_DIR})
    set(SRPC_LIBRARIES ${SRPC_LIBRARY})
endif ()