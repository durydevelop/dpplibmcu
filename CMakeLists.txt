cmake_minimum_required(VERSION 3.13)

#### Create project
project(dpplibmcu
    VERSION 0.2.0
    LANGUAGES CXX
)

#### Common libraries options
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
include(GNUInstallDirs)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS yes)

#### CMake modules helper
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
list (REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(DCMakeHelper)

#### Default options
if(CMAKE_VERSION VERSION_LESS 3.21)
    if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
        set(PROJECT_IS_TOP_LEVEL TRUE)
    else()
        set(PROJECT_IS_TOP_LEVEL FALSE)
    endif()
#[[ oppure
    get_property(not_top DIRECTORY PROPERTY PARENT_DIRECTORY)
    if(not_top)
        set(PROJECT_IS_TOP_LEVEL FALSE)
    else()
        set(PROJECT_IS_TOP_LEVEL TRUE)
    endif()
]]#
endif()

if (PROJECT_IS_TOP_LEVEL)
    option(BUILD_DRIVERS "Build drivers for some third party peripherals (sensors, etc)" ${GPIO_SUPPORT})
    option(${PROJECT_NAME}_INSTALL "Create install target" ${GPIO_SUPPORT})
    option(${PROJECT_NAME}_BUILD_DRIVERS "Build drivers for some third party peripherals (sensors, etc)" ON)
    option(${PROJECT_NAME}_INSTALL "Create install target" ON)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples (need BUILD_DRIVER to be ON)" ON)
    option(${PROJECT_NAME}_BUILD_TESTING "Build tests suite" ON)
else()
    option(${PROJECT_NAME}_BUILD_DRIVERS "Build drivers for some third party peripherals (sensors, etc)" OFF)
    option(${PROJECT_NAME}_INSTALL "Create install target" OFF)
    option(${PROJECT_NAME}_BUILD_EXAMPLES "Build examples (need BUILD_DRIVER to be ON)" OFF)
    option(${PROJECT_NAME}_BUILD_TESTING "Build tests suite" OFF)
endif()

#### USE_EXTERNAL_DMPACKET
# TODO: quando DMPacket sarà stand alone
option(USE_EXTERNAL_DMPACKET "Do not use embedded version of DMPacket library" OFF)
if (NOT TARGET dmpacket)
    message_c(${BOLD_MAGENTA} "${BOLD_MAGENTA}<${BOLD_CYAN}dmpacket${BOLD_MAGENTA}> build-in target added")
    set(USE_EXTERNAL_DMPACKET ON)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/external/dmpacket)
endif()

#### USE_EXTERNAL_LG
# If USE_EXTERNAL_LG is ON one of these variables can be specified:
# Use custom git repo, i.e.:
# NOT YET: set(lg_GIT_REPOSITORY https://github.com/joan2937/lg.git)
# Use a local folder, i.e.:
# get_filename_component(ld_ROOT ../../lg ABSOLUTE)
# If none specified:
# First:  Try to find it in system
# NOT YET: Second: Load from default git repo (https://github.com/joan2937/lg.git)
option(USE_EXTERNAL_LG "Do not use embedded version of lg library" ON)
find_package(lg)
option(GPIO_SUPPORT "Enable Gpio support if present." ${lg_FOUND})

#### Handle options
# Shared / static lib
if (${PROJECT_NAME}_BUILD_SHARED_LIBS OR BUILD_SHARED_LIBS)
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
else()
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
endif()
# GPIO support
if (GPIO_SUPPORT)
    set(${PROJECT_NAME}_GPIO_SUPPORT ON)
    set(GPIO_SUPPORT ON)
else()
    set(${PROJECT_NAME}_GPIO_SUPPORT OFF)
    set(GPIO_SUPPORT OFF)
endif()
# Examples
if (${PROJECT_NAME}_BUILD_EXAMPLES OR BUILD_EXAMPLES)
    set(${PROJECT_NAME}_BUILD_EXAMPLES ON)
    set(BUILD_EXAMPLES OFF) # to prevent Raylib to build his examples
else()
    set(${PROJECT_NAME}_BUILD_EXAMPLES OFF)
    set(BUILD_EXAMPLES OFF)
endif()
# Drivers
if (${PROJECT_NAME}_BUILD_DRIVERS OR BUILD_DRIVERS)
    set(${PROJECT_NAME}_BUILD_DRIVERS ON)
    set(BUILD_DRIVERS ON)
else()
    set(${PROJECT_NAME}_BUILD_DRIVERS OFF)
    set(BUILD_DRIVERS OFF)
endif()
# Tests
if (${PROJECT_NAME}_BUILD_TESTING OR BUILD_TESTING)
    set(${PROJECT_NAME}_BUILD_TESTING ON)
    set(BUILD_TESTING ON)
else()
    set(${PROJECT_NAME}_BUILD_TESTING OFF)
    set(BUILD_TESTING OFF)
endif()
# Install
if (${PROJECT_NAME}_INSTALL OR ${PROJECT_NAME_UPPER}_INSTALL)
    set(${PROJECT_NAME}_INSTALL ON)
    set(${PROJECT_NAME_UPPER}_INSTALL ON)
else()
    set(${PROJECT_NAME}_INSTALL OFF)
    set(${PROJECT_NAME_UPPER}_INSTALL OFF)
endif()
# lg external
if (${PROJECT_NAME}_USE_EXTERNAL_LG OR USE_EXTERNAL_LG)
    set(${PROJECT_NAME}_USE_EXTERNAL_LG ON)
    set(USE_EXTERNAL_LG ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_LG OFF)
    set(USE_EXTERNAL_LG OFF)
endif()
# DMPacket external
if (${PROJECT_NAME}_USE_EXTERNAL_DMPACKET OR USE_EXTERNAL_DMPACKET)
    set(${PROJECT_NAME}_USE_EXTERNAL_DMPACKET ON)
    set(USE_EXTERNAL_DMPACKET ON)
else()
    set(${PROJECT_NAME}_USE_EXTERNAL_DMPACKET OFF)
    set(USE_EXTERNAL_DMPACKET OFF)
endif()

#### Encourage user to specify a build type (e.g. Release, Debug, etc.), otherwise set it to Debug.
if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Debug' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Debug")
    endif()
endif()

#### Sources
# Source library files
set(${PROJECT_NAME}_SRC)
# Header library files
set(${PROJECT_NAME}_HDR)
# Include base dir
set(${PROJECT_NAME}_INCLUDE_DIRS)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dutils)

# Add GPIO_SUPPORT dependend modules
if(GPIO_SUPPORT)
    message_c(${BOLD_WHITE} "Library lg found, gpio control are enabled")
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddcmotor)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalbutton)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitalinput)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/ddigitaloutput)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dgpio)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/di2c)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dpwm)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src/dservo)
    # Drivers for some external peripherlas (sensors, etc)
    if (${PROJECT_NAME}_BUILD_DRIVERS)
        message_c("Building drivers for peripherals")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/drivers)
    endif()
    # Examples
    if (${PROJECT_NAME}_BUILD_EXAMPLES)
        message_c("Building examples")
        add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/examples)
    endif()
else()
    message_c(${BOLD_YELLOW} "Library <${BOLD_CYAN}lg${BOLD_YELLOW}> not found, gpio control are NOT enabled")
endif()

#### Setup target
add_library(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

if (CMAKE_VERSION VERSION_LESS 3.28)
    # Use PUBLIC_HEADER
    target_sources(${PROJECT_NAME} PUBLIC ${${PROJECT_NAME}_HDR})
    set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HDR}")
else()
    # Use FILE_SET for HEADERS
    target_sources(${PROJECT_NAME}
        PUBLIC
            FILE_SET HEADERS
            FILES ${${PROJECT_NAME}_HDR}
    )
endif()

# Alias
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

# Specify include directories for both compilation and installation process.
target_include_directories(${PROJECT_NAME}
    PUBLIC 
        "$<BUILD_INTERFACE:${${PROJECT_NAME}_INCLUDE_DIRS}>"
        "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>"
)

if (GPIO_SUPPORT)
    target_link_libraries(${PROJECT_NAME} PUBLIC lgpio)
endif()

if (CMAKE_VERSION VERSION_LESS 3.28)
    message_c(${BOLD_YELLOW} "Install is not supported, cannot create correct folder structure. You need to use cmake 3.28 or higher")
    set(${PROJECT_NAME}_INSTALL FALSE)
endif()

#### Install stuff
if (${${PROJECT_NAME}_INSTALL})
    # Enable RPATH support for installed binaries and libraries
    include(AddInstallRPATHSupport)
    add_install_rpath_support(
        BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"
        LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"
        INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
        USE_LINK_PATH
    )

    # Specify installation targets, typology and destination folders.
    if (CMAKE_VERSION VERSION_LESS 3.28)
        # Using PUBLIC_HEADER
        install(TARGETS ${PROJECT_NAME}
            EXPORT  ${PROJECT_NAME}
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}        COMPONENT dev
            LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT shlib
            ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT lib
            RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                     COMPONENT bin
            PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}" COMPONENT dev
        )
    else()
        # Using FILE_SET
        install(TARGETS ${PROJECT_NAME}
                EXPORT  ${PROJECT_NAME}
                FILE_SET HEADERS
                INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}        COMPONENT dev
                LIBRARY       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT shlib
                ARCHIVE       DESTINATION "${CMAKE_INSTALL_LIBDIR}"                     COMPONENT lib
                RUNTIME       DESTINATION "${CMAKE_INSTALL_BINDIR}"                     COMPONENT bin
        )
    endif()

    # Create and install CMake configuration files
    include(InstallBasicPackageFiles)
    install_basic_package_files(${PROJECT_NAME}
        VERSION ${${PROJECT_NAME}_VERSION}
        COMPATIBILITY AnyNewerVersion
        VARS_PREFIX ${PROJECT_NAME}
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

    # Add the uninstall target
    include(AddUninstallTarget)
endif()

## C++ Pre-processor variables
# dpplibmcu_VERSION
add_compile_definitions(${PROJECT_NAME}_VERSION="${PROJECT_VERSION}")
# DPPLIBMCU_VERSION
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
add_compile_definitions(${PROJECT_NAME_UPPER}_VERSION="${PROJECT_VERSION}")

## Print summary
message_c(${BOLD_CYAN} "Summary:")
message_c(${BOLD_CYAN} "Project name:             ${PROJECT_NAME} - Version: ${PROJECT_VERSION}")
message_c(${BOLD_CYAN} "PROJECT_IS_TOP_LEVEL:     ${PROJECT_IS_TOP_LEVEL}")
message_c(${BOLD_CYAN} "BUILD_SHARED_LIBS:        ${${PROJECT_NAME}_BUILD_SHARED_LIBS}")
message_c(${BOLD_CYAN} "GPIO enabled:             ${${PROJECT_NAME}_GPIO_SUPPORT}")
message_c(${BOLD_CYAN} "Build drivers:            ${${PROJECT_NAME}_BUILD_DRIVERS}")
message_c(${BOLD_CYAN} "Build examples:           ${${PROJECT_NAME}_BUILD_EXAMPLES}")
message_c(${BOLD_CYAN} "Build testing:            ${${PROJECT_NAME}_BUILD_TESTING} (NOT YET)")
