# modified version of https://github.com/nasa/channel-emulator/blob/master/cmake/Modules/FindLibNL.cmake

find_path(NL_INCLUDE_DIRS netlink/netlink.h
	/usr/include
	/usr/include/libnl3
	/usr/local/include
	/usr/local/include/libnl3
	${CMAKE_INCLUDE_PATH}
	${CMAKE_PREFIX_PATH}/include/libnl3
)

find_library(
	NL_LIBRARY 
	NAMES nl nl-3 
	PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(
	NL_ROUTE_LIBRARY 
	NAMES nl-route nl-route-3
	PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(
	NL_NETFILTER_LIBRARY 
	NAMES nl-nf nl-nf-3
	PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
)
find_library(
	NL_GENL_LIBRARY 
	NAMES nl-genl nl-genl-3
	PATHS /usr/lib /usr/lib64 /usr/local/lib /usr/local/lib64 /opt/local/lib /sw/lib ${CMAKE_LIBRARY_PATH} ${CMAKE_INSTALL_PREFIX}/lib
)

if (NL_INCLUDE_DIRS AND NL_LIBRARY)
	set(NL_FOUND TRUE)
endif (NL_INCLUDE_DIRS AND NL_LIBRARY)

if (NL_FOUND)
	if (NOT NL_FIND_QUIETLY)
		set(NL_LIBRARIES ${NL_LIBRARY} ${NL_ROUTE_LIBRARY} ${NL_NETFILTER_LIBRARY} ${NL_GENL_LIBRARY})
        message("Found netlink libraries:  ${NL_LIBRARIES}")
        message("Found netlink includes: ${NL_INCLUDE_DIRS}")
	endif (NOT NL_FIND_QUIETLY)
ELSE (NL_FOUND)
	if (NL_FIND_REQUIRED)
		message(FATAL_ERROR "Could not find netlink library.")
	endif (NL_FIND_REQUIRED)
endif (NL_FOUND)
