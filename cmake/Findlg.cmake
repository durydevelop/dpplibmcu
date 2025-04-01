#### Find Lg lib
#[[ TODO
function(get_package_from_git)
    message_c("Add ${ARGV1} as external project")
    include(ExternalProject)
    find_program(MAKE_EXE NAMES gmake nmake make)
    ExternalProject_Add(${ARGV0}
        GIT_REPOSITORY    ${ARGV1}
        #GIT_TAG           origin/release/1.2.3
        BUILD_COMMAND     make
    )
endfunction()
]]#
if (USE_EXTERNAL_LG)
    # Do not use internal Lg
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} is finding <${BOLD_CYAN}lg${BOLD_MAGENTA}>")
    if (lg_GIT_REPOSITORY)
        # GIT forced
        message_c(${BOLD_MAGENTA} "forced")
        get_package_from_git(lg ${lg_GIT_REPOSITORY})
        if (lg_FOUND)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> loaded custom repo")
        endif()
    elseif(lg_ROOT)
        # Local FORCED
        add_subdirectory(${lg_ROOT} _deps/lg)
    else()
        # Try to find in system
        # Find the path to the lg includes.
        find_path(
            lg_INCLUDE_DIR 
            NAMES lgpio.h rgpio.h
            HINTS /usr/local/include
        )

        # Find the lg libraries.
        find_library(
            lgpio_LIBRARY 
            NAMES liblgpio.so
            HINTS /usr/local/lib
        )
        find_library(
            rgpio_LIBRARY 
            NAMES librgpio.so
            HINTS /usr/local/lib
        )

        # Set the lg variables to plural form to make them accessible for 
        # the paramount cmake modules.
        set(lg_INCLUDE_DIRS ${lg_INCLUDE_DIR})
        set(lg_INCLUDES     ${lg_INCLUDE_DIR})
        set(lg_INCLUDE_DIRECTORIES ${lg_INCLUDE_DIR})

        # Handle REQUIRED, QUIET, and version arguments 
        # and set the <packagename>_FOUND variable.
        include(FindPackageHandleStandardArgs)
        find_package_handle_standard_args(
            lg 
            DEFAULT_MSG 
            lg_INCLUDE_DIR lgpio_LIBRARY rgpio_LIBRARY
        )

        if (lg_FOUND)
            #get_target_property(lg_INCLUDE_DIRECTORIES lg INTERFACE_INCLUDE_DIRECTORIES)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> found in system")
        else()
            #[[ TODO
            # Load from official repo
            message_c(${BOLD_MAGENTA} "loading from official repo")
            get_package_from_git(lg https://github.com/joan2937/lg.git)
            set(lg_FOUND TRUE)
            message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} <${BOLD_CYAN}lg${BOLD_MAGENTA}> loaded from official repo")
            ]]#
        endif()
    endif()
else()
    # Use embedded one
    message_c(${BOLD_MAGENTA} "${BOLD_WHITE}${PROJECT_NAME}${BOLD_MAGENTA} use <${BOLD_CYAN}lg${BOLD_MAGENTA}> embedded")
    add_subdirectory(external/lg)
    install(
        DIRECTORY ${lg_SOURCE_DIR}/lg
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_TARGET_NAME}"
        FILES_MATCHING PATTERN "*.h"
    )
endif()

#get_target_property(lg_SOURCE_DIR lg SOURCE_DIR)
#get_target_property(lg_INCLUDE_DIRECTORIES lg INTERFACE_INCLUDE_DIRECTORIES)

#print_var(lg)
#print_target_prop(lg)
