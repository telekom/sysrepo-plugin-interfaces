if (LIBYANGCPP_LIBRARIES AND LIBYANGCPP_INCLUDE_DIRS)
    set(LIBYANGCPP_FOUND TRUE)
else ()

    find_path(
        LIBYANGCPP_INCLUDE_DIR
        NAMES libyang-cpp/Module.hpp
        PATHS /usr/include /usr/local/include /opt/local/include /sw/include ${CMAKE_INCLUDE_PATH} ${CMAKE_INSTALL_PREFIX}/include
    )

    find_library(
        LIBYANGCPP_LIBRARY
        NAMES yang-cpp
        PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
    )

    if (LIBYANGCPP_INCLUDE_DIR AND LIBYANGCPP_LIBRARY)
        set(LIBYANGCPP_FOUND TRUE)
    else (LIBYANGCPP_INCLUDE_DIR AND LIBYANGCPP_LIBRARY)
        set(LIBYANGCPP_FOUND FALSE)
    endif (LIBYANGCPP_INCLUDE_DIR AND LIBYANGCPP_LIBRARY)

    set(LIBYANGCPP_INCLUDE_DIRS ${LIBYANGCPP_INCLUDE_DIR})
    set(LIBYANGCPP_LIBRARIES ${LIBYANGCPP_LIBRARY})
endif ()