## CMakeLists file for adding this library to a cmake project
## How to use:
## If the folder is inside project tree use add_subdirectory(<path to this folder>)
## If the folder is outside project tree use add_subdirectory(<path to this folder> /deps/libdpp-mcu)

cmake_minimum_required(VERSION 3.13)

## Create project
project(dpplib-mcu
    VERSION 0.0.8
    LANGUAGES CXX
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_DRIVERS "Build drivers for some external peripherlas (sensors, etc)" ON)

## Cmake modules helper
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DMainHelper)
include(DPrintHelpers)
#include(SetupDoxygen)
include(FeatureSummary)

## Add platform indipendent modules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dmpacket)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dutils)

## Add lg dependend modules
find_package(lg)
if(lg_FOUND)
    message_c(${BOLD_WHITE} "Library lg found, gpio control are enabled")
    add_subdirectory(src/dgpio)
    add_subdirectory(src/ddigitalinput)
    add_subdirectory(src/ddigitaloutput)
    add_subdirectory(src/ddigitalbutton)
    add_subdirectory(src/dpwm)
    add_subdirectory(src/dservo)
    add_subdirectory(src/di2c)
else()
    message_c(${BOLD_WHITE} "Library pigpio not found, gpio control are NOT enabled")
endif()

## Drivers for some external peripherlas (sensors, etc)
if (BUILD_DRIVERS)
    message_c("Builing drivers for peripherals")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drivers)
endif()

## Setup Target
if(BUILD_SHARED_LIBS)
    # Building shared library
    if(MSVC)
        # Workaround for some projects
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()

    add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

    # Set library version
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${CMAKE_PROJECT_VERSION})

else()
    # Static library
    add_library(${PROJECT_NAME} ${SRC_FILES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIRS})
endif()
## TODO: interface
#set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
#set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

if(lp_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC lgpio)
endif()

if (BUILD_EXAMPLES)
    message_c("Building examples")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
    # ...also force external peripherals drivers
    set(BUILD_DRIVERS ON)
endif()

#include(DInstall)
add_subdirectory(doxygen)
print_project_summary()
feature_summary(WHAT ALL)