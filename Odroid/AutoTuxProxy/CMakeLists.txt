# Set the CMake version for Ivos standard (Linux mint...)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project name
PROJECT(AutoTuxProxy)

SET(CMAKE_MODULE_PATH "/opt/od/share/cmake-2.8/Modules" ${CMAKE_MODULE_PATH})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11 -lusb-1.0 -lopencv_core -lopencv_highgui -lopencv_imgproc")
# set flags here for cmake
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O -Wall -Weffc++ -pedantic")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Waggregate-return -Wcast-align")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcast-qual -Wchar-subscripts")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wcomment -Wconversion -Wdisabled-optimization")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wformat -Wformat=2")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-nonliteral -Wformat-security")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wformat-y2k -Wimplicit -Wimport")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self -Winline -Winvalid-pch")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunsafe-loop-optimizations -Wlong-long")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-braces -Wmissing-field-initializers")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-format-attribute -Wmissing-include-dirs")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-noreturn -Wpacked -Wpadded -Wparentheses")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith -Wredundant-decls -Wreturn-type")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsequence-point -Wshadow -Wsign-compare -Wstack-protector")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch-default -Wswitch-enum -Wtrigraphs -Wuninitialized")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunknown-pragmas -Wunreachable-code -Wunused")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-function -Wunused-label -Wunused-parameter")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunused-value -Wunused-variable")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wvariadic-macros -Wvolatile-register-var -Wwrite-strings")

# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

# Find Automotivedata
SET(AUTOMOTIVEDATA_DIR "/opt/od")
FIND_PACKAGE(AutomotiveData REQUIRED)

# Find OpenCV
#SET(OPENCV_ROOT_DIR "/usr")
FIND_PACKAGE(OpenCV REQUIRED)

# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES(SYSTEM ${OPENDAVINCI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${AUTOMOTIVEDATA_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${LIBUSB_INCLUDE_DIR})
# Set include directory.
INCLUDE_DIRECTORIES(include)

SET(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${OpenCV_LIBS} ${LIBUSB_LIBRARIES} usb-1.0)

FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h")
ADD_LIBRARY(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
ADD_EXECUTABLE(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})
