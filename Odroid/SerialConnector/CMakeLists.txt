cmake_minimum_required(VERSION 3.3)
project(SerialConnector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#IVO
set(CME_CXX_FLAGS "${CMAKE_CXX_FLAGS} pkg-config libusb-1.0 --libs --cflags")

#JONAS

set(SOURCE_FILES main.cpp)
add_executable(SerialConnector ${SOURCE_FILES})