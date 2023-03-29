if (AUGYANG_LIBRARIES)
  # in cache already
  set(AUGYANG_FOUND TRUE)
else(AUGYANG_LIBRARIES)
    find_library(
        AUGYANG_LIBRARY
    NAMES
        srds_augeas.so
    PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /usr/local/lib64
        /opt/local/lib
        /sw/lib
        ${CMAKE_LIBRARY_PATH}
        ${CMAKE_INSTALL_PREFIX}/lib

        # srds_augeas.so
        ${CMAKE_INSTALL_PREFIX}/lib/sysrepo/plugins
    )

    if (AUGYANG_LIBRARY)
        set(AUGYANG_FOUND TRUE)
    else (AUGYANG_LIBRARY)
        set(AUGYANG_FOUND FALSE)
    endif (AUGYANG_LIBRARY)

    set(AUGYANG_LIBRARIES ${AUGYANG_LIBRARY})

    mark_as_advanced(AUGYANG_LIBRARIES)
endif(AUGYANG_LIBRARIES)