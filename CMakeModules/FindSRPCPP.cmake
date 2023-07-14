if (SRPCPP_LIBRARIES AND SRPCPP_INCLUDE_DIRS)
    set(SRPCPP_FOUND TRUE)
else ()

    find_path(
        SRPCPP_INCLUDE_DIR
        NAMES srpcpp.hpp
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        SRPCPP_LIBRARY
        NAMES srpcpp
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if (SRPCPP_INCLUDE_DIR AND SRPCPP_LIBRARY)
        set(SRPCPP_FOUND TRUE)
    else (SRPCPP_INCLUDE_DIR AND SRPCPP_LIBRARY)
        set(SRPCPP_FOUND FALSE)
    endif (SRPCPP_INCLUDE_DIR AND SRPCPP_LIBRARY)

    set(SRPCPP_INCLUDE_DIRS ${SRPCPP_INCLUDE_DIR})
    set(SRPCPP_LIBRARIES ${SRPCPP_LIBRARY})
endif ()