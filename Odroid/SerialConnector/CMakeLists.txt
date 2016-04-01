cmake_minimum_required(VERSION 2.8)
project(SerialConnector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#IVO
set(SOURCE_FILES src/main.cpp src/serial/SerialConnector.cpp src/serial/SerialConnector.cpp)
add_executable(main ${SOURCE_FILES})
target_link_libraries(main ${LIBUSB_LIBRARIES})

#JONAS
