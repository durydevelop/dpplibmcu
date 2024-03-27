## CMakeLists file for adding this library to a cmake project
## How to use:
## If the folder is inside project tree use add_subdirectory(<path to this folder>)
## If the folder is outside project tree use add_subdirectory(<path to this folder> /deps/libdpp-mcu)

cmake_minimum_required(VERSION 3.13)

## Create project
project(dpplib-mcu
    VERSION 0.0.1
    LANGUAGES CXX
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#set(CMAKE_CXX_STANDARD 20)

## Cmake modules helper
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
include(DMainHelper)
include(DPrintHelpers)

## Setup Target
message(${DSTATUS} "Configuring ${PROJECT_NAME} library...")
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dmpacket)

## pgpio
find_package(pigpio)

#set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
#set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)

add_library(${PROJECT_NAME} ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIRS})

if (pigpio_FOUND)
    message_c("pigpio lib found, enable gpio")
    add_subdirectory(src/dpplib-gpio)
    add_subdirectory(src/ddigitalinput)
    target_link_libraries(${PROJECT_NAME} PUBLIC pigpio)
else()
    message_c("pigpio lib not found, gpio is not enabled")
endif()
