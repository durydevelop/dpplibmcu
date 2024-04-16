#add_executable(dmpacket)

if(lg_FOUND)
    ## Add gpio examples
    # io-demo
    add_executable(io-demo ${CMAKE_CURRENT_SOURCE_DIR}/ddigitalio/sbc-io-demo/main.cpp)
    target_link_libraries(io-demo PUBLIC dpplib-mcu)
    target_link_libraries(io-demo PUBLIC lgpio)

    # pwm-demo
    add_executable(pwm-demo ${CMAKE_CURRENT_SOURCE_DIR}/dpwm/sbc-pwm-demo/main.cpp)
    target_link_libraries(pwm-demo PUBLIC dpplib-mcu)
    target_link_libraries(pwm-demo PUBLIC lgpio)

    # servo-demo
    add_executable(servo-demo ${CMAKE_CURRENT_SOURCE_DIR}/dservo/sbc-servo-demo/main.cpp)
    target_link_libraries(servo-demo PUBLIC dpplib-mcu)
    target_link_libraries(servo-demo PUBLIC lgpio)
    
    # i2c-bus-info
    add_executable(i2c-bus-info
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/i2c-bus-info.cpp
    )
    target_link_libraries(i2c-bus-info PUBLIC dpplib-mcu)

    # ina226 / ina228
    add_executable(ina226
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/ina226.cpp
    )
    add_executable(ina228
        ${CMAKE_CURRENT_SOURCE_DIR}/i2c/sbc-i2c-demo/ina228.cpp
    )
    target_link_libraries(ina226 PUBLIC dpplib-mcu)
    target_link_libraries(ina228 PUBLIC dpplib-mcu)

    #target_link_libraries(rpi-i2c-demo PUBLIC lgpio)
endif()
