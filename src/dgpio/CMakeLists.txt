set(SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpiochip.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpio.cpp
)

set(HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpiochip
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpiochip.h
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpio
    ${CMAKE_CURRENT_SOURCE_DIR}/dgpio.h
    ${CMAKE_CURRENT_SOURCE_DIR}/derrorcodes.h
)

set(${PROJECT_NAME}_SRC ${${PROJECT_NAME}_SRC} ${SRC} PARENT_SCOPE)
set(${PROJECT_NAME}_HDR ${${PROJECT_NAME}_HDR} ${HDR} PARENT_SCOPE)
set(${PROJECT_NAME}_INCLUDE_DIRS "${${PROJECT_NAME}_INCLUDE_DIRS}" ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
