######
## Common usefull stuffs

######
# Colors defines
include(DCMakeColors)

# Set stuff for stand-alone or lib project
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Stand-alone project
    set(DSTATUS ">> ")
	# Disable console popup
	if(WIN32)
		set(GUI_TYPE WIN32)
		message(${DSTATUS} "Set WIN32 gui")
	elseif(APPLE)
		set(GUI_TYPE MACOSX_BUNDLE)
		message(${DSTATUS} "Set APPLE gui")
	endif()
else()
    # Part of other project
    set(DSTATUS ">>>> ")
    message_c(${BOLD_CYAN} "Library <${PROJECT_NAME}> included as external project <<<<")
endif()

# CMake helper scripts
include(DPrintHelpers)
include(generate_product_version)

message_c(${BOLD_GREEN} "CMake version: ${CMAKE_VERSION}")
message_c(${BOLD_GREEN} "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_CXX_STANDARD)
	message_c(${BOLD_GREEN} "C++ std: ${CMAKE_CXX_STANDARD}")
endif()

# set everything up for c++ 20 features
#set(CMAKE_CXX_STANDARD 20)
#if(MSVC)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++23")
#endif(MSVC)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

