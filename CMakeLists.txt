cmake_minimum_required(VERSION 3.13)

## Create project
project(dpplibmcu
    VERSION 0.1.0
    LANGUAGES CXX
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if (UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-sign-compare")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wshorten-64-to-32")
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    # Build with Multiple Processes
    target_compile_options(${PROJECT_NAME} PRIVATE /MP)
endif()

## Cmake modules helper
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DCMakeHelper)

## Default options
find_package(lg QUIET)
option(GPIO_SUPPORT "Enable Gpio support if present." ${lg_FOUND})
option(BUILD_DRIVERS "Build drivers for some third party peripherals (sensors, etc)" ${STAND_ALONE_LIB})
option(BUILD_EXAMPLES "Build examples (need BUILD_DRIVER to be ON)" ${BUILD_DRIVERS})

## Handle options
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
endif()

## Add Platform indipendent modules
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dmpacket)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dutils)

## Add GPIO_SUPPORT dependend modules
if(GPIO_SUPPORT)
    message_c(${BOLD_WHITE} "Library lg found, gpio control are enabled")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dgpio)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalinput)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitaloutput)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalbutton)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dpwm)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dservo)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/di2c)
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

## Setup target
if(BUILD_SHARED_LIBS)
    # Building shared library
    if(MSVC)
        # Workaround for some projects
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
    endif()
    add_library( ${PROJECT_NAME} SHARED
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
    )
    # Set library version
    set_target_properties(${PROJECT_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
else()
    # Static library
    add_library(${PROJECT_NAME}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADERS}
    )
endif()
# Includes
message_c("INCLUDE_DIRS=${${PROJECT_NAME}_INCLUDE_DIRS}")
include(GNUInstallDirs)
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIRS}/>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}>
)
set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HEADERS}")
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

#[[
## Doxygen support
if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    # Only if stand-alone project
    set(DOXYGEN_THEME custom1)
    set(DOXYGEN_LANGUAGE Italian)
    #add_subdirectory(doxygen)
    list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/doxygen)
    include(Doxygen)
endif()
]]#

include(DLibInstall)

if(GPIO_SUPPORT)
    target_link_libraries(${PROJECT_NAME} PUBLIC lgpio)
endif()

## C++ Pre-processor variables
# dpplib_VERSION
add_compile_definitions(${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# DPPLIB_VERSION
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "Build examples:           ${BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build tests:              ${BUILD_TESTS}")
message_c(${BOLD_CYAN} "Build Qt extension:       ${BUILD_QT}")
message_c(${BOLD_CYAN} "Build networking support: ${BUILD_NET}")
