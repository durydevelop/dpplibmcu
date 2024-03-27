## CMakeLists file for adding this library to a cmake project

cmake_minimum_required(VERSION 3.13)

## Create project
project(dpplib-mcu
    VERSION 0.0.1
    LANGUAGES CXX
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_CXX_STANDARD 20)

## Cmake modules helper
#list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
#include(DMainHelper)
#include(DPrintHelpers)

## Setup Target
message(${DSTATUS} "Configuring ${PROJECT_NAME} library...")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_library(${PROJECT_NAME} INTERFACE) # ${SRC_FILES})

target_include_directories(${PROJECT_NAME} INTERFACE ${SRC_DIRS})

set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

#[[
## How to use:
## If the folder is inside project tree use add_subdirectory(<path to this folder>)
## If the folder is outside project tree use add_subdirectory(<path to this folder> /deps/libdpp-mcu)

message(${DSTATUS} "Configuring ${PROJECT_NAME} library...")

# Add sources to project
set(FILES
	${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalinput/DDigitalInput.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalinput/DDigitalInput.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalinput/ddigitalinput
	${CMAKE_CURRENT_SOURCE_DIR}/src/dpplib-gpio/dpplib-gpio.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/src/dpplib-gpio/dpplib-gpio.h
	${CMAKE_CURRENT_SOURCE_DIR}/src/dpplib-gpio/dpplib-gpio
)

# Propagate sources to parent scope
set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
message(${DSTATUS} "Found ${LIB_NAME} in ${CMAKE_CURRENT_SOURCE_DIR}/src")
#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Need cmake 3.13 or policy CMP0079 set to NEW to link a library from a different cmake module
if((CMAKE_MINIMUM_REQUIRED_VERSION VERSION_LESS 3.13) AND (CMAKE_VERSION VERSION_GREATER_EQUAL 3.13))
    cmake_policy(SET CMP0079 NEW)
endif()

#message(${DSTATUS} "Link ${LIB_NAME}: ${FILES}")
#target_sources(${PROJECT_NAME} PRIVATE ${FILES})
]]#
