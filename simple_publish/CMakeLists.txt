cmake_minimum_required(VERSION 3.6)

get_filename_component(ProjectId ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" ProjectId ${ProjectId})
project(${ProjectId}  VERSION 1.1.0)

# Add Project Files
add_executable(${PROJECT_NAME} main.c)

################################################################################
# Following lines required for EdgeDevX
add_subdirectory("EdgeDevX" edge_devx)
target_link_libraries(${PROJECT_NAME} "edge_devx")
################################################################################

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
