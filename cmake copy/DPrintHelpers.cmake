########################################################################
## Print all cmake VARIABLES.
# If an argument is given, only varialbles containing ARGV0 are printed.
function(print_var)
    message("#### Variables founds containing " ${ARGV0} " ####")
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    list(REMOVE_DUPLICATES _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH "[a-zA-Z]*${ARGV0}[a-zA-Z]*" MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        set(FOUND TRUE)
        message("## ${_variableName}=${${_variableName}}")
    endforeach()
    if (NOT FOUND)
        message("## none founds")
    endif()
endfunction()

########################################################################
## Print all TARGET VARIABLES
# Varialble are listed using COMMAND cmake --help-property-list
# If an argument is given, only varialbles containing ARGV0 (${tgt}) are printed.
function(print_target_prop tgt)
    message("#### Variables founds for TARGET " ${tgt} " ####")
    if (ARGV1)
        message("## containing " ${ARGV1})
    endif()
    if(NOT TARGET ${tgt})
        message("## There is no target named '${tgt}'")
        return()
    endif()

    ## Get all properties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    ## Convert command output into a CMake list
    STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    foreach (prop ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${CMAKE_BUILD_TYPE}" prop ${prop})
        if (${prop} MATCHES "(LOCATION)")
            # Skip prop contains LOCATION due to the POLICY restriction
            continue()
        endif()
        if (ARGV1)
            unset(MATCHED)
            string(REGEX MATCH "[a-zA-Z]*${ARGV1}[a-zA-Z]*" MATCHED ${prop})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        set(FOUND TRUE)
        get_target_property(propval ${tgt} ${prop})
        if (propval)
            message ("## ${tgt}::${prop} = ${propval}")
        endif()
    endforeach(prop)
    if (NOT FOUND)
        message("## none founds")
    endif()
endfunction()

########################################################################
## Print all TARGET VARIABLES
# Varialble are listed using get_cmake_property(CMAKE_PROPERTY_LIST VARIABLES)
# If an argument is given, only varialbles containing ARGV0 (${tgt}) are printed.
function(print_target_set tgt)
    message("#### Variables founds for TARGET " ${tgt} " ####")
    if(NOT TARGET ${tgt})
        message("## There is no target named '${tgt}'")
        return()
    endif()

    ## Get all properties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)
    ## Convert command output into a CMake list
    STRING(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    STRING(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    list(REMOVE_DUPLICATES CMAKE_PROPERTY_LIST)
    foreach(prop ${CMAKE_PROPERTY_LIST})
        if (${prop} MATCHES "(LOCATION)")
            # Skip prop contains LOCATION due to the POLICY restriction
            continue()
        endif()
        # v for value, d for defined, s for set
        get_property(v TARGET ${tgt} PROPERTY ${prop})
        get_property(d TARGET ${tgt} PROPERTY ${prop} DEFINED)
        get_property(s TARGET ${tgt} PROPERTY ${prop} SET)
        # only produce output for values that are set
        if(s)
            message("## ${tgt}::${prop}")
            message("## Value=${v}")
            message("## Defined=${d}")
            message("## Set=${s}")
            message("")
        endif()
    endforeach()
endfunction()
