cmake_minimum_required(VERSION 3.16)
project(comm)
add_definitions("-Wall -std=c++17")

# include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
# conan_basic_setup()

# message(STATUS "Check the boost 1.74")
set(Boost_ROOT ${PROJECT_SOURCE_DIR}/boost/boost_1_74_0)
set(Boost_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/boost/boost_1_74_0)
set(Boost_LIBRARY_DIR ${PROJECT_SOURCE_DIR}/boost/boost_1_74_0/stage/lib)
find_package(Boost COMPONENTS filesystem system iostreams serialization REQUIRED)

message(STATUS "PThread..")
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/../)
include_directories(${Boost_INCLUDE_DIR})
message(STATUS "Root DIR:" ${PROJECT_SOURCE_DIR}/../)

file(GLOB_RECURSE source_list 
	"comm.hpp" "Context.cpp" "Context.h"
	"app/*.hpp" "app/*.cpp" "app/*.h" 
	"presentation/*.hpp" "presentation/*.cpp" "presentation/*.h"
	"transport/*.hpp" "transport/*.cpp" "transport/*.h")

# add_executable(comm ${source_list} "Main.cpp")
add_library(comm STATIC ${source_list} "UDPClient.cpp" "UDPClient.h" "UDPServer.cpp" "UDPServer.h" "TCPClient.h"
		"TCPClient.cpp" "TCPServer.cpp" "TCPServer.h" "Ctx.cpp" "Ctx.h")
target_link_libraries(comm ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})