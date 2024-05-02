cmake_minimum_required (VERSION 2.8)

# https://github.com/kracejic/EmptyDoxygenCMake/tree/master?tab=readme-ov-file
# This CMake file will add doc target for convenient documentation generation.
#
# Additional features are supported:
#   * dot = for automatically generated UML diagrams (class, include, caller, call, collaboration)
#   * PlantUML = for easy generation of custom diagrams

# Helper functions
macro(path_linux_to_win MsysPath ResultingPath)
  string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()
macro(path_win_to_linux MsysPath ResultingPath)
  string(REGEX REPLACE "^([a-zA-Z]):/" "/\\1/" ${ResultingPath} "${MsysPath}")
endmacro()

find_package(Doxygen
             REQUIRED dot
             OPTIONAL_COMPONENTS mscgen dia)
message_c(${BOLD_MAGENTA} "Doxygen found ${DOXYGEN_FOUND}")
if(DOXYGEN_FOUND)

set(DOXYGEN_PROJECT_NAME ${MAKE_PROJECT_NAME})
set(DOXYGEN_PROJECT_NUMBER ${VERSION})

set(DOXYGEN_HTML_EXTRA_STYLESHEET ${CMAKE_CURRENT_SOURCE_DIR}/custom1/customdoxygen.css)
set(DOXYGEN_HTML_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/custom1/header.html)
set(DOXYGEN_HTML_FOOTER ${CMAKE_CURRENT_SOURCE_DIR}/custom1/footer.html)

set(DOXYGEN_EXCLUDE_PATTERNS *_)
set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_GENERATE_LATEX NO)
set(DOXYGEN_QUIET YES)
set(DOXYGEN_WARN_IF_UNDOCUMENTED NO)
set(DOXYGEN_WARN_NO_PARAMDOC NO)
set(DOXYGEN_STRIP_FROM_PATH src)
set(DOXYGEN_STRIP_FROM_INC_PATH src)

# Use SHORT_NAMES to prevent Doxygen from generating filenames with double-quotes.
# ALLOW_UNICODE_NAMES does not escape double-quotes.
set(DOXYGEN_SHORT_NAMES YES)

set(DOXYGEN_JAVADOC_AUTOBRIEF YES)
set(DOXYGEN_ALWAYS_DETAILED_SEC YES)
set(DOXYGEN_DISTRIBUTE_GROUP_DOC YES)
 
# https://cmake.org/cmake/help/latest/module/FindDoxygen.html#command:doxygen_add_docs
    doxygen_add_docs(doc
      src
      doc
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#	  CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/DoxyFile
      COMMENT "Generate Documentation"
    )
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc DESTINATION share COMPONENT doc EXCLUDE_FROM_ALL)
endif()
