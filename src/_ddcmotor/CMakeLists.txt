set(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/ddcmotor
    ${CMAKE_CURRENT_SOURCE_DIR}/ddcmotor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ddcmotor.h
)

set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
