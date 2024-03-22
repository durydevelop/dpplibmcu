cmake_minimum_required(VERSION 3.5)

project(rpi-input-output-demo LANGUAGES CXX)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()


message(">> CMake version: ${CMAKE_VERSION}")
message(">> gcc c++ version: ${CMAKE_CXX_COMPILER_VERSION}")

# set everything up for c++ 17 features
set(CMAKE_CXX_STANDARD 17)
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif(MSVC)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

add_executable(${PROJECT_NAME}
    main.cpp
)

add_subdirectory(../../.. _dep/libdpp-mcu)
target_sources(${PROJECT_NAME} PUBLIC ${SRC_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SRC_DIRS})

#pgpio
find_package(pigpio REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE pigpio)
