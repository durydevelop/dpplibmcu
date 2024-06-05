set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
if (NOT ixwebsocket_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        ixwebsocket
        GIT_REPOSITORY https://github.com/machinezone/IXWebSocket.git
    )
	# Check if population has already been performed
	FetchContent_GetProperties(ixwebsocket)
	if (NOT ixwebsocket_POPULATED)
		set(FETCHCONTENT_QUIET OFF)
		FetchContent_Populate(ixwebsocket)
		add_subdirectory(${ixwebsocket_SOURCE_DIR} ${ixwebsocket_BINARY_DIR})
	endif()
	FetchContent_MakeAvailable(ixwebsocket)
	
endif()

include_directories(${PROJECT_NAME} ${ixwebsocket_SOURCE_DIR})
#target_link_libraries(${PROJECT_NAME} ixwebsocket)