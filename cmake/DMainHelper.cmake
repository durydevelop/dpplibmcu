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
    message_c(${BOLD_MAGENTA} "Library ${PROJECT_NAME} included as external project <<<<")
endif()

# CMake helper scripts
include(DPrintHelpers)
include(generate_product_version)

message_c(${BOLD_GREEN} "CMake version: ${CMAKE_VERSION}")
message_c(${BOLD_GREEN} "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
if (CMAKE_CXX_STANDARD)
	message_c(${BOLD_GREEN} "C++ std: ${CMAKE_CXX_STANDARD}")
endif()

# Project summary
function (print_project_summary)
    message_c(${BOLD_CYAN} "Summary: ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
    message_c(${BOLD_CYAN} "Build type: ${CMAKE_BUILD_TYPE}")

    cmake_host_system_information(RESULT DISTRO QUERY OS_NAME OS_PLATFORM OS_RELEASE)

    foreach(VAR IN LISTS DISTRO)
    set(OS_INFO "${OS_INFO} ${VAR}")
    endforeach()
    message_C(${BOLD_CYAN} "Operating System: ${OS_INFO}")
endfunction()
