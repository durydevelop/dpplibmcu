# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#[=======================================================================[.rst:
DLibraryType
---------

Set some variables for handle library types as INTERFACE, SHARED or STATIC.

Use this module by invoking :command:`include` with the form:

.. code-block:: cmake

  set(LIBRARY_TYPE <INTERFACE | SHARED | STATIC>)
  include(DLibraryType)

Result Variables
^^^^^^^^^^^^^^^^

If ``LIBRARY_TYPE`` is set to ``INTERFACE``:
``${PROJECT_NAME}_BUILD_SHARED_LIBS`` and ``BUILD_SHARED_LIBS`` are set to ``OFF``.
``${PROJECT_NAME}_BUILD_INTERFACE_LIBS`` and ``BUILD_INTERFACE_LIBS`` are set to ``ON``.

If ``LIBRARY_TYPE`` is set to ``SHARED``:
``${PROJECT_NAME}_BUILD_SHARED_LIBS`` and ``BUILD_SHARED_LIBS`` are set to ``ON``.
``${PROJECT_NAME}_BUILD_INTERFACE_LIBS`` and ``BUILD_INTERFACE_LIBS`` are set to ``OFF``.

If ``LIBRARY_TYPE`` is set to ``STATIC``:
``${PROJECT_NAME}_BUILD_SHARED_LIBS`` and ``BUILD_SHARED_LIBS`` are set to ``OFF``.
``${PROJECT_NAME}_BUILD_INTERFACE_LIBS`` and ``BUILD_INTERFACE_LIBS`` are set to ``OFF``.

These variables can be used to handle add_library()

Examples
^^^^^^^^

Set current library as interface:

.. code-block:: cmake

  set(LIBRARY_TYPE INTERFACE)
  include(DLibraryType)

Set current library as interface:

.. code-block:: cmake

  set(LIBRARY_TYPE INTERFACE)
  include(DLibraryType)

#]=======================================================================]

if (NOT ${PROJECT_NAME}_LIBRARY_TYPE)
    ## default
    set(${PROJECT_NAME}_LIBRARY_TYPE INTERFACE)
endif()

if (${PROJECT_NAME}_LIBRARY_TYPE MATCHES INTERFACE)
    # INTERFACE has prececence
    # Set INTERFACE ON
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS ON)
    set(BUILD_INTERFACE_LIBS ON)
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
elseif (${PROJECT_NAME}_LIBRARY_TYPE MATCHES SHARED)
    # SHARED
    # Set SHARED ON
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS ON)
    set(BUILD_SHARED_LIBS ON)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
else()
    # STATIC
    # Set SHARED OFF
    set(${PROJECT_NAME}_BUILD_SHARED_LIBS OFF)
    set(BUILD_SHARED_LIBS OFF)
    # Set INTERFACE OFF
    set(${PROJECT_NAME}_BUILD_INTERFACE_LIBS OFF)
    set(BUILD_INTERFACE_LIBS OFF)
endif()
