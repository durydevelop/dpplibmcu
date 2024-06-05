set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
if (NOT raygui_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        raygui
        GIT_REPOSITORY https://github.com/raysan5/raygui.git
        GIT_TAG 3.6
    )
	# Check if population has already been performed
	FetchContent_GetProperties(raygui)
	if(NOT raygui_POPULATED)
        set(FETCHCONTENT_QUIET OFF)
		FetchContent_Populate(raygui)
		# TODO: add_subdirectory(${raygui_SOURCE_DIR} ${raygui_BINARY_DIR})
	endif()
	FetchContent_MakeAvailable(raygui)
	
endif()

include_directories(${PROJECT_NAME} ${raygui_SOURCE_DIR}/src)