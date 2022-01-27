cmake_minimum_required(VERSION 3.6)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}  VERSION 1.1.0)

include(RemoteX.cmake)

# Add Project Files
add_executable(${PROJECT_NAME} main.c)


################################################################################
# Following lines required for AzureSphereRemoteX
add_subdirectory("AzureSphereRemoteX" applibs)
target_link_libraries(${PROJECT_NAME} "applibs")
################################################################################

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
