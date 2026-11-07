# With USE_EXTERNAL_LG=ON, specify one of these options (checked in this order):
# Clone and build a Git repository:
# set(lg_GIT_REPOSITORY https://github.com/joan2937/lg.git)
# Optional branch, tag, or commit (defaults to master):
# set(lg_GIT_TAG master)
# Or use a local source directory containing a CMakeLists.txt:
# get_filename_component(lg_ROOT ../../lg ABSOLUTE)
# If neither is specified, search for an installed lg library.
# With USE_EXTERNAL_LG=OFF, use the bundled source instead.

include(FindPackageHandleStandardArgs)

# Select an external installation/source or the bundled copy of lg.
if(USE_EXTERNAL_LG)
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}lg${BOLD_MAGENTA}>")

    if(lg_GIT_REPOSITORY)
        # Upstream lg uses a Makefile rather than CMake; build its libraries as an external project.
        include(ExternalProject)

        find_program(lg_MAKE_EXECUTABLE NAMES gmake make)
        if(NOT lg_MAKE_EXECUTABLE)
            message(FATAL_ERROR
                "A make executable (gmake or make) is required to build lg_GIT_REPOSITORY."
            )
        endif()

        set(_lg_source_dir "${CMAKE_BINARY_DIR}/_deps/lg-src")
        file(MAKE_DIRECTORY "${_lg_source_dir}")
        # Allow pinning a branch, tag, or commit; otherwise use the upstream default branch.
        if(lg_GIT_TAG)
            set(_lg_git_tag "${lg_GIT_TAG}")
        else()
            set(_lg_git_tag master)
        endif()

        ExternalProject_Add(lg_external
            GIT_REPOSITORY "${lg_GIT_REPOSITORY}"
            GIT_TAG "${_lg_git_tag}"
            SOURCE_DIR "${_lg_source_dir}"
            CONFIGURE_COMMAND ""
            BUILD_IN_SOURCE TRUE
            BUILD_COMMAND "${lg_MAKE_EXECUTABLE}" lib
            INSTALL_COMMAND ""
            BUILD_BYPRODUCTS
                "${_lg_source_dir}/liblgpio.so"
                "${_lg_source_dir}/librgpio.so"
        )

        if(TARGET lgpio)
            message(FATAL_ERROR
                "Cannot build lg_GIT_REPOSITORY because a target named 'lgpio' already exists."
            )
        endif()

        # Expose lgpio as an imported target so consumers wait for the external build.
        add_library(lgpio UNKNOWN IMPORTED GLOBAL)
        set_target_properties(lgpio PROPERTIES
            IMPORTED_LOCATION "${_lg_source_dir}/liblgpio.so"
            INTERFACE_INCLUDE_DIRECTORIES "${_lg_source_dir}"
        )
        add_dependencies(lgpio lg_external)

        set(lg_INCLUDE_DIR "${_lg_source_dir}")
        set(lg_INCLUDE_DIRS "${_lg_source_dir}")
        set(lg_INCLUDES "${_lg_source_dir}")
        set(lg_INCLUDE_DIRECTORIES "${_lg_source_dir}")
        set(lgpio_LIBRARY "${_lg_source_dir}/liblgpio.so")
        set(rgpio_LIBRARY "${_lg_source_dir}/librgpio.so")
        set(lg_LIBRARIES lgpio)

        set(lg_FOUND TRUE)
        message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> will be cloned from ${lg_GIT_REPOSITORY} (${_lg_git_tag})")
    elseif(lg_ROOT)
        # Use a caller-provided lg checkout that has its own CMake project.
        if(NOT EXISTS "${lg_ROOT}/CMakeLists.txt")
            message(FATAL_ERROR
                "lg_ROOT='${lg_ROOT}' does not contain a CMakeLists.txt file."
            )
        endif()

        add_subdirectory("${lg_ROOT}" "${CMAKE_BINARY_DIR}/_deps/lg")
        set(lg_FOUND TRUE)
        message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> loaded from local source")
    else()
        # Fall back to an lg installation available through the normal system search paths.
        find_path(
            lg_INCLUDE_DIR
            NAMES lgpio.h
        )

        find_library(
            lgpio_LIBRARY
            NAMES lgpio
        )

        set(lg_INCLUDE_DIRS "${lg_INCLUDE_DIR}")
        set(lg_INCLUDES "${lg_INCLUDE_DIR}")
        set(lg_INCLUDE_DIRECTORIES "${lg_INCLUDE_DIR}")
        set(lg_LIBRARIES "${lgpio_LIBRARY}")

        find_package_handle_standard_args(
            lg
            DEFAULT_MSG
            lg_INCLUDE_DIR
            lgpio_LIBRARY
        )

        if(lg_FOUND)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> found in system")
        endif()
    endif()
else()
    # The bundled source is stored relative to this find-module, not the caller's directory.
    set(_lg_source_dir "${CMAKE_CURRENT_LIST_DIR}/../src/external/lg")
    if(NOT EXISTS "${_lg_source_dir}/CMakeLists.txt")
        message(FATAL_ERROR
            "The embedded lg source is missing or incomplete at '${_lg_source_dir}'. "
            "Initialize or restore that source, or configure with USE_EXTERNAL_LG=ON."
        )
    endif()

    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}lg${BOLD_MAGENTA}> embedded")
    add_subdirectory("${_lg_source_dir}" "${CMAKE_BINARY_DIR}/_deps/lg")
    set(lg_FOUND TRUE)

    # Install bundled headers when that source tree provides them.
    if(EXISTS "${_lg_source_dir}/lg")
        install(
            DIRECTORY "${_lg_source_dir}/lg"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}"
            FILES_MATCHING PATTERN "*.h"
        )
    endif()
endif()
