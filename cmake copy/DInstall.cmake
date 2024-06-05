# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
Install.cmake
-----------------
Generic script that add install command to your project.
It create folder structure from CMAKE_INSTALL_PREFIX position
${CMAKE_INSTALL_PREFIX}|
                       |-include|
                       |        \-${PROJECT_NAME}|
                       |                         |-header1.h
                       |                         |-header2.h
                       |                         |.
                       |                         |-CMakeLists.txt
                       |                         |.
                       |
                       \-lib|
                            |-cmake|
                            |      |-${PROJECT_NAME}Config.cmake
                            |      \-${PROJECT_NAME}ConfigVersion.cmake
                            |
                            \-lib${PROJECT_NAME}.a
#]=======================================================================]

#include(GenerateExportHeader)
#generate_export_header(${PROJECT_NAME})

include(GNUInstallDirs)

# Config.cmake
set(INCLUDE_INSTALL_DIR include)
set(LIB_INSTALL_DIR lib)
set(SYSCONFIG_INSTALL_DIR etc/${PROJECT_NAME})

include(CMakePackageConfigHelpers)
configure_package_config_file(
    ${CONFIG_CMAKE_IN_PATH}/Config.cmake.in                    # in file
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake # out file
    INSTALL_DESTINATION
        ${LIB_INSTALL_DIR}/${PROJECT_NAME}/cmake
    PATH_VARS
        INCLUDE_INSTALL_DIR
        SYSCONFIG_INSTALL_DIR
)

# ConfigVersion.cmake
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        SameMajorVersion
)

# Install directives
install(
    TARGETS
        ${PROJECT_NAME}
    EXPORT
        ${PROJECT_NAME}Targets
    INCLUDES DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    PUBLIC_HEADER DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
    CONFIGURATIONS
        Debug
        RUNTIME DESTINATION
            Debug/bin
)

install(
    TARGETS
        ${PROJECT_NAME}
    CONFIGURATIONS
        Release
        RUNTIME DESTINATION
            Release/bin
)

install(
    DIRECTORY
        ${CMAKE_SOURCE_DIR}/include/
    DESTINATION
        ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake
    DESTINATION
        ${LIB_INSTALL_DIR}/cmake
)
#[[
add_custom_command(
        TARGET foo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/test/input.txt
                ${CMAKE_CURRENT_BINARY_DIR}/input.txt
)
#]]
