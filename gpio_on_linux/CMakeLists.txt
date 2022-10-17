cmake_minimum_required(VERSION 3.6)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId} VERSION 1.1.0)

# Keeping this example light and just including the dx_gpio support

set(ALL_FILES
    main.c
    EdgeDevX/src/dx_gpio.c
)

# Add Project Files
add_executable(${PROJECT_NAME} ${ALL_FILES})

# ###############################################################################
# Following lines required for EdgeDevX
# add_subdirectory("EdgeDevX" edge_devx)
# target_link_libraries(${PROJECT_NAME} "edge_devx")

target_include_directories(${PROJECT_NAME} PUBLIC
    EdgeDevX/include
)

target_link_libraries (${PROJECT_NAME} gpiod)

# ###############################################################################
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
