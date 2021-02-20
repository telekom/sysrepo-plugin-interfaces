option(ENABLE_LOG_LOG "Enable log messages" ON)
option(ENABLE_LOG_WARNING "Enable warning messages" ON)
option(ENABLE_LOG_ERROR "Enable error messages" ON)
option(ENABLE_LOG_DEBUG1 "Enable level 1 debug messages" ON)
option(ENABLE_LOG_DEBUG2 "Enable level 2 debug messages" OFF)
option(ENABLE_LOG_DEBUG3 "Enable level 3 debug messages" OFF)

if(ENABLE_LOG_LOG)
	add_definitions(-DLOG)
endif()

if(ENABLE_LOG_WARNING)
	add_definitions(-DWARNING)
endif()

if(ENABLE_LOG_ERROR)
	add_definitions(-DERROR)
endif()

if(ENABLE_LOG_DEBUG1)
	add_definitions(-DDEBUG1)
endif()

if(ENABLE_LOG_DEBUG2)
	add_definitions(-DDEBUG2)
endif()

if(ENABLE_LOG_DEBUG3)
	add_definitions(-DDEBUG3)
endif()