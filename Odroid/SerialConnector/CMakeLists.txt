cmake_minimum_required(VERSION 2.8)
project(SerialConnector)
# IVO
include(FindPkgConfig)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#IVO
pkg_check_modules (LIBUSB REQUIRED libusb-1.0)
include_directories (${LIBUSB_INCLUDE_DIRS})
link_directories (${LIBUSB_LIBRARY_DIRS})
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} pkg-config libusb-1.0 --libs --cflags")
set(SOURCE_FILES src/main.cpp src/serial/SerialConnector.cpp src/serial/SerialConnector.cpp)
add_executable(main ${SOURCE_FILES})

#JONAS
