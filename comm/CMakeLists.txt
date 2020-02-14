cmake_minimum_required(VERSION 3.5.2)
project(comm)
add_definitions("-Wall -std=c++17 -O2")

message(STATUS "Check the boost 1.72")
set(BOOST_INSTALL /home/ubuntu/boost)
set(BOOST_INCLUDE_DIR ${BOOST_INSTALL}/boost_1_72_0)
set(BOOST_LIB_DIR ${BOOST_INCLUDE_DIR}/stage/lib)
set(BOOST_LIBRARIES ${BOOST_LIB_DIR}/libboost_filesystem.a ${BOOST_LIB_DIR}/libboost_system.a)

message(STATUS "PThread..")
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${BOOST_INCLUDE_DIR})
message(STATUS "Root DIR:" ${PROJECT_SOURCE_DIR}/../)

file(GLOB_RECURSE source_list 
	"comm.hpp" "Context.cpp" "Context.h"
	"app/*.hpp" "app/*.cpp" "app/*.h" 
	"presentation/*.hpp" "presentation/*.cpp" "presentation/*.h"
	"transport/*.hpp" "transport/*.cpp" "transport/*.h")

# add_executable(comm ${source_list} "Main.cpp")
add_library(comm STATIC ${source_list} "UDPClient.cpp" "UDPClient.h" "UDPServer.cpp" "UDPServer.h" "Ctx.cpp" "Ctx.h")
target_link_libraries(comm ${BOOST_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})