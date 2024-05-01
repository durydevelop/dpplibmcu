######
# DPPLIB
##[[ Sorgenti in locale
## usa DPPLIB_ROOT
message_c(${BOLD_MAGENTA} "Finding dpplib...")
if (NOT ${dpplib_ROOT} STREQUAL "")
    message_c(${BOLD_MAGENTA} "dpplib_ROOT set to ${dpplib_ROOT}")
endif()

if (NOT EXISTS ${dpplib_ROOT})
    message(FATAL_ERROR "dpplib_ROOT not found. Please set correct dpplib_ROOT path")
endif()
##]]

#[[ Sorgenti da git
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
set(FETCHCONTENT_BASE_DIR deps)
FetchContent_Declare(
    DPPLIB
    GIT_REPOSITORY https://dury:CFg3BdZs3Wt8zeVobxCc@gitlab.com/durydevelop/cpp/lib/DPPLIB.git
    #GIT_PROGRESS TRUE
)
# Check if population has already been performed
FetchContent_GetProperties(DPPLIB)
if(NOT DPPLIB_POPULATED)
    message(${DSTATUS} "Not populated")
    FetchContent_Populate(DPPLIB)
endif()
FetchContent_MakeAvailable(DPPLIB)

add_subdirectory(${DPPLIB_ROOT} deps/DPPLIB)
include_directories(${DPPLIB_ROOT}/include)
target_link_libraries(${PROJECT_NAME} PRIVATE DPPLIB)
#]]

add_subdirectory(${dpplib_ROOT} _deps/dpplib)
target_include_directories(${PROJECT_NAME} PUBLIC ${dpplib_ROOT}/include)
get_target_property(DPPLIB_SOURCES dpplib SOURCES)
target_sources(${PROJECT_NAME} PUBLIC ${DPPLIB_SOURCES})

#include_directories(${PROJECT_NAME} PUBLIC ${Boost_INCLUDE_DIR})

# std::filesystem
#target_link_libraries(${PROJECT_NAME}
#    PUBLIC
#        stdc++fs
#        pthread
#)

# winsock
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    message_c("Linking Winsock2 for " ${PROJECT_NAME})
    target_link_libraries(${PROJECT_NAME} PUBLIC ws2_32 wsock32)
endif()
