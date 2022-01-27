cmake_minimum_required(VERSION 3.6)
project(avnet_end_to_end VERSION 0.1.0)

# message(STATUS "directory")
# message(STATUS ${CMAKE_CURRENT_LIST_DIR})

# Add Project Files
add_executable(${PROJECT_NAME} main.c sensor.c logger.c)
target_link_libraries(${PROJECT_NAME} applibs)

include(RemoteX.cmake)

set(ENABLE_DESKTOP_DEVX_REMOTEX ON CACHE  BOOL "ENABLE DesktopDevX RemoteX peripheral support" FORCE )

################################################################################
# Following lines required for DesktopDevX
add_subdirectory("DesktopDevX" desktop_devx)
target_link_libraries(${PROJECT_NAME} "desktop_devx")
################################################################################



add_subdirectory("AzureSphereDrivers/AVNET/HL" avnet_drivers)
target_link_libraries(${PROJECT_NAME} avnet_drivers)

add_subdirectory("AzureSphereDrivers/GROVE_HM3301" hm330x_driver)
target_link_libraries(${PROJECT_NAME} hm330x_driver)

add_subdirectory("AzureSphereDrivers/CLICK_8x8_LED_MAX7219" max7219_driver)
target_link_libraries(${PROJECT_NAME} max7219_driver)

add_subdirectory("AzureSphereDrivers/CLICK_8800_RETRO_AS1115" as1115_driver)
target_link_libraries(${PROJECT_NAME} as1115_driver)

add_subdirectory("AzureSphereDrivers/CLICK_EEPROM2" eeprom2_driver)
target_link_libraries(${PROJECT_NAME} eeprom2_driver)

add_subdirectory("AzureSphereDrivers/CLICK_4x4_KEY_74HC165" 74HC165_driver)
target_link_libraries(${PROJECT_NAME} 74HC165_driver)


# Set hardware definitons

set(BOARD_COUNTER 0)

if(AVNET)
    MATH(EXPR BOARD_COUNTER "${BOARD_COUNTER}+1")
    add_definitions( -DOEM_AVNET=TRUE )
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/HardwareDefinitions/avnet_mt3620_sk/inc/")
    message(STATUS "Azure Sphere board selected: AVNET REV 1")
endif(AVNET)

if(AVNET_REV_2)
    MATH(EXPR BOARD_COUNTER "${BOARD_COUNTER}+1")
    add_definitions( -DOEM_AVNET=TRUE )
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/HardwareDefinitions/avnet_mt3620_sk_rev2/inc/")
    message(STATUS "Azure Sphere board selected: AVNET REV 2")
endif(AVNET_REV_2)

if (SEEED_STUDIO_RDB)
    MATH(EXPR BOARD_COUNTER "${BOARD_COUNTER}+1")
    add_definitions( -DOEM_SEEED_STUDIO=TRUE )
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/HardwareDefinitions/mt3620_rdb/inc/")
    message(STATUS "Azure Sphere board selected: SEEED STUDIO RDB")
endif()

if (SEEED_STUDIO_MINI)
    MATH(EXPR BOARD_COUNTER "${BOARD_COUNTER}+1")
    add_definitions( -DOEM_SEEED_STUDIO_MINI=TRUE )    
    target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/HardwareDefinitions/seeed_mt3620_mdb/inc/")
    message(STATUS "Azure Sphere board selected: SEEED STUDIO MINI")
endif(SEEED_STUDIO_MINI)

if(BOARD_COUNTER EQUAL 0)
    message(FATAL_ERROR "No Azure Sphere boards selected. Ensure one board set")
endif()

if(BOARD_COUNTER GREATER 1)
    message(FATAL_ERROR "Multiple (${BOARD_COUNTER}) Azure Sphere boards selected. Ensure only one board set")
endif()


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
