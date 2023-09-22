if(PTHREAD_LIBRARIES AND PTHREAD_INCLUDE_DIRS)
    set(PTHREAD_FOUND TRUE)
else()
    find_path(
        PTHREAD_INCLUDE_DIR
        NAMES pthread.h
        PATHS /usr/include
              /usr/local/include
              /opt/local/include
              /sw/include
              ${CMAKE_INCLUDE_PATH}
              ${CMAKE_INSTALL_PREFIX}/include)

    find_library(
        PTHREAD_LIBRARY
        NAMES pthread
        PATHS /usr/lib
              /usr/lib64
              /usr/local/lib
              /usr/local/lib64
              /opt/local/lib
              /sw/lib
              ${CMAKE_LIBRARY_PATH}
              ${CMAKE_INSTALL_PREFIX}/lib)

    if(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
        set(PTHREAD_FOUND TRUE)
    else(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)
        set(PTHREAD_FOUND FALSE)
    endif(PTHREAD_INCLUDE_DIR AND PTHREAD_LIBRARY)

    set(PTHREAD_INCLUDE_DIRS ${PTHREAD_INCLUDE_DIR})
    set(PTHREAD_LIBRARIES ${PTHREAD_LIBRARY})
endif()
