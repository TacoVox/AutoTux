CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(SerialConnector)

SET (CMAKE_MODULE_PATH "/opt/od/share/cmake-3.5/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11")
# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES (${OPENDAVINCI_INCLUDE_DIRS})
# Set include directory.
INCLUDE_DIRECTORIES(include)
# Build this project.
SET(SOURCE_FILES src/SerialConnector.cpp src/packetio/PacketBroadcaster.cpp src/packetio/PacketBroadcaster.h src/packetio/PacketReceiver.cpp src/packetio/PacketReceiver.h src/serial/SerialConnector.cpp src/serial/SerialConnector.cpp)
ADD_EXECUTABLE (SerialConnector ${SOURCE_FILES})

# Linking
TARGET_LINK_LIBRARIES (SerialConnector ${OPENDAVINCI_LIBRARIES} ${LIBUSB_LIBRARIES})
