# Set the CMake version for Ivos standard (Linux mint...)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project name
PROJECT(AutoTuxProxy)

SET (CMAKE_MODULE_PATH "/opt/od/share/cmake-2.8/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11 -lusb-1.0")
# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES(${OPENDAVINCI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${LIBUSB_INCLUDE_DIR})
# Set include directory.
INCLUDE_DIRECTORIES(include)

# Set all the source files // Ordered by namespace
# file containing main
SET(SOURCE_FILES src/AutoTuxProxy.cpp)
# serial
SET(SOURCE_FILES ${SOURCE_FILES} src/serial/USBConnector.cpp src/serial/USBConnector.h)
# packetio
SET(SOURCE_FILES ${SOURCE_FILES} src/packetio/PacketBroadcaster.cpp include/packetio/PacketBroadcaster.h src/packetio/PacketReceiver.cpp include/packetio/PacketReceiver.h)
# containerfactory
#SET(SOURCE_FILES ${SOURCE_FILES} src/containerfactory/SBDContainer.cpp include/containerfactory/SBDContainer.h)

# Set executable
ADD_EXECUTABLE (AutoTuxProxy ${SOURCE_FILES})

# Link the libraries to the exec.
TARGET_LINK_LIBRARIES (AutoTuxProxy ${OPENDAVINCI_LIBRARIES} ${LIBUSB_LIBRARIES})
