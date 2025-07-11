if(lg_FOUND)
    ## Add gpio examples

    # button-demo
    add_executable(button-demo ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalio/sbc-button-demo/main.cpp)
    target_link_libraries(button-demo PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(button-demo PUBLIC lgpio)
    
    # io-demo
    add_executable(io-demo ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalio/sbc-io-demo/main.cpp)
    target_link_libraries(io-demo PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(io-demo PUBLIC lgpio)

    # pwm-demo
    add_executable(pwm-demo ${CMAKE_CURRENT_SOURCE_DIR}/dpwm/sbc-pwm-demo/main.cpp)
    target_link_libraries(pwm-demo PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(pwm-demo PUBLIC lgpio)

    # servo-demo
    add_executable(servo-demo ${CMAKE_CURRENT_SOURCE_DIR}/dservo/sbc-servo-demo/main.cpp)
    target_link_libraries(servo-demo PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(servo-demo PUBLIC lgpio)

    # i2c-bus-list
    add_executable(i2c-bus-list
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/i2c-bus-list.cpp
    )
    target_link_libraries(i2c-bus-list PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(i2c-bus-list PUBLIC dmpacket::dmpacket)
    
    # i2c-bus-info
    add_executable(i2c-bus-info
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/i2c-bus-info.cpp
    )
    target_link_libraries(i2c-bus-info PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(i2c-bus-info PUBLIC dmpacket::dmpacket)

    # i2c-dev-scan
    add_executable(i2c-scan
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/i2c-scan.cpp
    )
    target_link_libraries(i2c-scan PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(i2c-scan PUBLIC dmpacket::dmpacket)

    # i2c-ask
    add_executable(i2c-ask
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/i2c-ask.cpp
    )
    target_link_libraries(i2c-ask PUBLIC dpplibmcu::dpplibmcu)
    target_link_libraries(i2c-ask PUBLIC dmpacket::dmpacket)

    # ina226 (current / voltage sensor)
    add_executable(ina226
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/ina226.cpp
    )
    target_link_libraries(ina226 PUBLIC dpplibmcu::dpplibmcu)

    # ina228 (current / voltage sensor)
    add_executable(ina228
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/ina228.cpp
    )
    target_link_libraries(ina228 PUBLIC dpplibmcu::dpplibmcu)

endif()
