if(CMOCKA_LIBRARIES AND CMOCKA_INCLUDE_DIRS)
    set(CMOCKA_FOUND TRUE)
else()

    find_path(
        CMOCKA_INCLUDE_DIR
        NAMES cmocka
        PATHS /usr/include
              /usr/local/include
              /opt/local/include
              /sw/include
              ${CMAKE_INCLUDE_PATH}
              ${CMAKE_INSTALL_PREFIX}/include)

    find_library(
        CMOCKA_LIBRARY
        NAMES cmocka libcmocka
        PATHS /usr/lib
              /usr/lib64
              /usr/local/lib
              /usr/local/lib64
              /opt/local/lib
              /sw/lib
              ${CMAKE_LIBRARY_PATH}
              ${CMAKE_INSTALL_PREFIX}/lib)

    if(CMOCKA_INCLUDE_DIR AND CMOCKA_LIBRARY)
        set(CMOCKA_FOUND TRUE)
    else(CMOCKA_INCLUDE_DIR AND CMOCKA_LIBRARY)
        set(CMOCKA_FOUND FALSE)
    endif(CMOCKA_INCLUDE_DIR AND CMOCKA_LIBRARY)

    set(CMOCKA_INCLUDE_DIRS ${CMOCKA_INCLUDE_DIR})
    set(CMOCKA_LIBRARIES ${CMOCKA_LIBRARY})
endif()
