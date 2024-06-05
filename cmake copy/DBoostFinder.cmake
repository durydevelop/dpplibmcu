# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
DBoostFinder
---------

Find and set all stuff for Boost library

Use this module by invoking :command:`include` with the form:

.. code-block:: cmake

  set(Boost_FIND_ARGS [BOOST_FIND_ARGS])
  include(DBoostFinder)

  Where BOOST_FIND_ARGS can be the same options used when call 'find_package' command for Boost:

    [version] [EXACT]      # Minimum or EXACT version e.g. 1.67.0
    [REQUIRED]             # Fail with error if Boost is not found
    [COMPONENTS <libs>...] # Boost libraries by their canonical name
                           # e.g. "date_time" for "libboost_date_time"
    [OPTIONAL_COMPONENTS <libs>...]
                           # Optional Boost libraries by their canonical name
                           # e.g. "date_time" for "libboost_date_time"

    See :command:`FindBoost` for details.

Result Variables
^^^^^^^^^^^^^^^^

All variables defined by :command:`FindBoost` are set.

Hints
^^^^^

The all in one variable 'BOOST_FIND_ARGS' can be used set all the BOOST_... variables
that wil be used by pass to :command:`FindBoost`.

For example you can set:

``BOOST_ROOT``, ``BOOSTROOT``
  Preferred installation prefix.

``BOOST_INCLUDEDIR``
  Preferred include directory e.g. ``<prefix>/include``.

``BOOST_LIBRARYDIR``
  Preferred library directory e.g. ``<prefix>/lib``.

``Boost_NO_SYSTEM_PATHS``
  Set to ``ON`` to disable searching in locations not
  specified by these hint variables. Default is ``OFF``.

``Boost_ADDITIONAL_VERSIONS``
  List of Boost versions not known to this module.
  (Boost install locations may contain the version).

  See :command:`FindBoost` for details.

Examples
^^^^^^^^

Simply find Boost in system path:

.. code-block:: cmake

  include(DBoostFinder)

Find Boost libraries specifing custom path, minimum version, and required component:

.. code-block:: cmake

  get_filename_component(BOOST_ROOT_DIR "../../../../lib/boost" ABSOLUTE) # same as BOOST_ROOT_DIR "../../../../lib/boost"
  set (Boost_FIND_ARGS 1.75 REQUIRED COMPONENTS filesystem)
  set(Boost_ROOT ${BOOST_ROOT_DIR})
  include(DBoostFinder)

#]=======================================================================]

#set(Boost_USE_STATIC_LIBS OFF) 
#set(Boost_USE_MULTITHREADED ON)  
#set(Boost_USE_STATIC_RUNTIME OFF)

message(${DSTATUS} "Project <${PROJECT_NAME}> is finding boost library...")
if ((NOT DEFINED Boost_FOUND) OR (NOT ${Boost_FOUND}))
	if (NOT ${Boost_ROOT} STREQUAL "")
        set(Boost_NO_SYSTEM_PATHS ON)
		message(${DSTATUS} "Boost_ROOT forced to " ${Boost_ROOT})
	endif()
	if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
		message(${DSTATUS} "\tfor Windows")
	elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
		message(${DSTATUS} "\tFor Linux")
	endif()
	find_package(Boost ${BOOST_FIND_ARGS})
endif()

if (${Boost_FOUND})
    ## headers only
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
    message(${DSTATUS} "\tFound ${Boost_VERSION_STRING}")
    message(${DSTATUS} "\tBoost include dir: ${Boost_INCLUDE_DIR}")
    if (NOT ${Boost_LIBRARY_DIR} STREQUAL "")
        message(${DSTATUS} "\tBoost lib: ${Boost_LIBRARY_DIR}")
	endif()
else()
	message(FATAL_ERROR ${DSTATUS} "Boost not found")
endif()
