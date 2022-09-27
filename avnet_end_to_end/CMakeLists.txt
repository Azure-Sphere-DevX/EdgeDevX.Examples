cmake_minimum_required(VERSION 3.6)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}  VERSION 1.1.0)

include(RemoteX.cmake)

set(ENABLE_EDGE_DEVX_REMOTEX ON CACHE  BOOL "ENABLE EdgeDevX RemoteX peripheral support" FORCE )

# Enable or disable RemoteX Peripheral support
if (ENABLE_EDGE_DEVX_REMOTEX)

    set(Peripherals
        "EdgeDevX/src/dx_gpio.c"
        "EdgeDevX/src/dx_i2c.c"
        "EdgeDevX/src/dx_pwm.c"
    )
    source_group("Peripherals" FILES ${Peripherals})

endif(ENABLE_EDGE_DEVX_REMOTEX)

set(ALL_FILES
    main.c
    ${Peripherals}
)

# Add Project Files
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_link_libraries(${PROJECT_NAME} applibs)

################################################################################
# Following lines required for EdgeDevX
add_subdirectory("EdgeDevX" edge_devx)
target_link_libraries(${PROJECT_NAME} "edge_devx")
################################################################################

# Enable or disable RemoteX Peripheral support
if (ENABLE_EDGE_DEVX_REMOTEX)
   # ################################################################################
   # # The following line required for the AzureSphereRemoteX.Client library
   add_subdirectory("AzureSphereRemoteX" applibs)
   target_link_libraries(${PROJECT_NAME} applibs)
   # ################################################################################
endif(ENABLE_EDGE_DEVX_REMOTEX)

# Enable or disable RemoteX Peripheral support
if (ENABLE_EDGE_DEVX_REMOTEX)
   message(STATUS "RemoteX Peripherals Enabled")
   # Following line required for Azure Sphere RemoteX Client library
   target_include_directories(${PROJECT_NAME} PUBLIC AzureSphereRemoteX )
else (ENABLE_EDGE_DEVX_REMOTEX)
   message(STATUS "RemoteX Peripherals Disabled")
endif(ENABLE_EDGE_DEVX_REMOTEX)


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
