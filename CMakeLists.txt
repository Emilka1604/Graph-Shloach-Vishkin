cmake_minimum_required(VERSION 3.21)

project(igraph_test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(igraph REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC igraph::igraph)


