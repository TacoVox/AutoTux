cmake_minimum_required(VERSION 3.3)
project(SerialConnector)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#IVO
set(CME_CXX_FLAGS "${CMAKE_CXX_FLAGS} pkg-config libusb-1.0 --libs --cflags")

#JONAS
# OpenDaVINCI Stuff - BE CAREFUL - Jonas' setup
SET (CMAKE_MODULE_PATH "/opt/od/share/cmake-3.5/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe")
# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES (${OPENDAVINCI_INCLUDE_DIRS})
# Set include directory.
INCLUDE_DIRECTORIES(include)
# Build this project.
SET(SOURCE_FILES main.cpp src/packetio/PacketBroadcaster.cpp src/packetio/PacketBroadcaster.h src/packetio/PacketReceiver.cpp src/packetio/PacketReceiver.h)
ADD_EXECUTABLE (main ${SOURCE_FILES})
TARGET_LINK_LIBRARIES (main ${OPENDAVINCI_LIBRARIES})

#add_executable(SerialConnector ${SOURCE_FILES})