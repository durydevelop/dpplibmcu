set(FILES
    #${CMAKE_CURRENT_SOURCE_DIR}/dsoftpwm
    ${CMAKE_CURRENT_SOURCE_DIR}/dpwm
)

if(lg_FOUND)
    add_subdirectory(sbc)
else()
    add_subdirectory(avr)
endif()

set(SRC_FILES ${SRC_FILES} ${FILES} PARENT_SCOPE)
set(SRC_DIRS ${SRC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} PARENT_SCOPE)
