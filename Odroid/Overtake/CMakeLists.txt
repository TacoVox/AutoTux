mum_required(VERSION 3.4)
project(LaneFollower)

# Set module paths, system dependant.
# TODO Make cmake search for it
set(CMAKE_MODULE_PATH "/opt/od/share/cmake-3.5/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lopencv_core -lopencv_highgui")

# Require OpenDaVINCI
set(OPENDAVINCI_DIR "/opt/od/")
find_package(OpenDaVINCI REQUIRED)

# Require Automotivedata
set(AUTOMOTIVEDATA_DIR "/opt/od")
find_package(AutomotiveData REQUIRED)

# Find OpenCV.
set(OPENCV_ROOT_DIR "/usr")
find_package(OpenCV REQUIRED)

set(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${OPENCV_LIBRARIES})

# Include dirs
include_directories(${OPENDAVINCI_INCLUDE_DIRS})
include_directories(${AUTOMOTIVEDATA_INCLUDE_DIRS})
include_directories(${OPENCV_INCLUDE_DIRS})
include_directories(include)

# Build things?
file(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/miniature COMPONENT software)
