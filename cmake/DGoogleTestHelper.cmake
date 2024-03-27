# Setup GoolgeTest library

message(">> Configuring GoogleTest library")
enable_testing()
include(FetchContent)
set(FETCHCONTENT_QUIET OFF)
set(FETCHCONTENT_UPDATES_DISCONNECTED OFF)
set(FETCHCONTENT_BASE_DIR deps)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
#set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
#print_var(googletest)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
#[[
    print_var(googletest)
    set(GOOGLETEST_DIR "${googletest-distribution_SOURCE_DIR}")
    if (EXISTS ${GOOGLETEST_DIR})
        set(GTestSrc ${GOOGLETEST_DIR}/googletest)
        set(GMockSrc ${GOOGLETEST_DIR}/googlemock)
    elseif (UNIX AND EXISTS /usr/src/gtest)
        set(GTestSrc /usr/src/gtest)
        message(WARNING "Using gtest from system")
        if (EXISTS /usr/src/gmock)
            set(GMockSrc /usr/src/gmock)
        endif ()
    else ()
        message( FATAL_ERROR "No googletest src dir found - set GOOGLETEST_DIR to enable!")
    endif ()

    set(GTestFiles ${GTestSrc}/src/gtest-all.cc)
    set(GTestIncludes ${GTestSrc} ${GTestSrc}/include)
    if (NOT ${GMockSrc} STREQUAL "")
        list(APPEND GTestFiles ${GMockSrc}/src/gmock-all.cc)
        list(APPEND GTestIncludes ${GMockSrc} ${GMockSrc}/include)
    endif ()

    include_directories(${GTestIncludes})
#]]
