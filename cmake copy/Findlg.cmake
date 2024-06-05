################################################################################
### Find the lg shared libraries.
################################################################################

# Find the path to the lg includes.
find_path(lg_INCLUDE_DIR 
	NAMES lgpio.h rgpio.h
	HINTS /usr/local/include)
	
# Find the lg libraries.
find_library(lgpio_LIBRARY 
	NAMES liblgpio.so
	HINTS /usr/local/lib)
find_library(rgpio_LIBRARY 
	NAMES librgpio.so
	HINTS /usr/local/lib)
    
# Set the lg variables to plural form to make them accessible for 
# the paramount cmake modules.
set(lg_INCLUDE_DIRS ${lg_INCLUDE_DIR})
set(lg_INCLUDES     ${lg_INCLUDE_DIR})

# Handle REQUIRED, QUIET, and version arguments 
# and set the <packagename>_FOUND variable.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(lg 
    DEFAULT_MSG 
    lg_INCLUDE_DIR lgpio_LIBRARY rgpio_LIBRARY)
