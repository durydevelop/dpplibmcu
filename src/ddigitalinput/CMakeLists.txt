set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalinput.cpp
)

set(HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalinput
    ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalinput.h
)

set(${PROJECT_NAME}_SRC ${${PROJECT_NAME}_SRC} ${SRC} PARENT_SCOPE)
set(${PROJECT_NAME}_HDR ${${PROJECT_NAME}_HDR} ${HDR} PARENT_SCOPE)
set(${PROJECT_NAME}_INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIRS}" ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
