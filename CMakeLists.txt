cmake_minimum_required(VERSION 3.21)

project(igraph_test)

# set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/includes)
# set(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# file(GLOB PROJECT_HEADERS ${DAEMON_INCLUDE_DIR}/*.h)
# file(GLOB PROJECT_SOURCE ${DAEMON_SOURCE_DIR}/*.cpp)

# set(PROJECT_HEADERS_LIST ${DAEMON_INCLUDE_DIR})

# include_directories(${PROJECT_HEADERS_LIST})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes)

add_executable(${PROJECT_NAME} src/main.cpp)

find_package(igraph REQUIRED)

target_link_libraries(${PROJECT_NAME} PUBLIC igraph::igraph)


