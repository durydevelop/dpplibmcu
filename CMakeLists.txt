## CMakeLists file for adding this library to a cmake project
## How to use:
## If the folder is inside project tree use add_subdirectory(<path to this folder>)
## If the folder is outside project tree use add_subdirectory(<path to this folder> /deps/libdpp-mcu)

cmake_minimum_required(VERSION 3.13)

## Create project
project(dpplib-mcu
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)

## Cmake modules helper
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DMainHelper)
include(DPrintHelpers)
include(FeatureSummary)

## Handle options
if (${CMAKE_CURRENT_SOURCE_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    set(STAND_ALONE_LIB ON)
endif()
find_package(lg QUIET)
option(GPIO_SUPPORT "Enable Gpio support if present." ${lg_FOUND})
option(BUILD_DRIVERS "Build drivers for some third party peripherals (sensors, etc)" ${STAND_ALONE_LIB})
option(BUILD_EXAMPLES "Build examples (need BUILD_DRIVER to be ON)" ${BUILD_DRIVERS})
#set(BUILD_SHARED_LIBS OFF) # Build shared libraries (.dll/.so) instead of static ones (.lib/.a)

# Handle STATIC/SHARED/INTERFACE build option
include(DLibType)

## Add platform indipendent modules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dmpacket)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dutils)

## Add GPIO_SUPPORT dependend modules
if(GPIO_SUPPORT)
    message_c(${BOLD_WHITE} "Library lg found, gpio control are enabled")
    add_subdirectory(src/dgpio)
    add_subdirectory(src/ddigitalinput)
    add_subdirectory(src/ddigitaloutput)
    add_subdirectory(src/ddigitalbutton)
    add_subdirectory(src/dpwm)
    add_subdirectory(src/dservo)
    add_subdirectory(src/di2c)
    # Drivers for some external peripherlas (sensors, etc)
    if (BUILD_DRIVERS)
        message_c("Building drivers for peripherals")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drivers)
    endif()
    # Examples
    if (BUILD_EXAMPLES)
        message_c("Building examples")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
    endif()
else()
    message_c(${BOLD_YELLOW} "Library pigpio not found, gpio control are NOT enabled")
endif()

## Setup Target
if (BUILD_INTERFACE_LIB)
    # Build as interface
    add_library(${PROJECT_NAME} INTERFACE)
    ## TODO: interface
    #set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
    #set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
elseif(BUILD_SHARED_LIBS)
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

## Doxygen support
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Only if stand-alone project
    set(DOXYGEN_THEME custom1)
    set(DOXYGEN_LANGUAGE Italian)
    #add_subdirectory(doxygen)
    list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
    include(Doxygen)
endif()

if(GPIO_SUPPORT)
    target_link_libraries(${PROJECT_NAME} PUBLIC lgpio)
endif()

#include(DInstall)

message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Gpio support:        ${GPIO_SUPPORT}")
message_c(${BOLD_CYAN} "Build as:            ${LIB_TYPE} LIB")
message_c(${BOLD_CYAN} "Build drivers:       ${BUILD_DRIVERS}")
message_c(${BOLD_CYAN} "Build examples:      ${BUILD_EXAMPLES}")
#message_c(${BOLD_CYAN} "Build tests:         ${BUILD_TESTS}")

#print_project_summary()
#feature_summary(WHAT ALL)
