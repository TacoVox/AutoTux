cmake_minimum_required(VERSION 3.2)
project(Overtaking)

# Set module paths, system dependant.
# TODO Make cmake search for it
set(CMAKE_MODULE_PATH "/usr/local/share/cmake-3.2/Modules" ${CMAKE_MODULE_PATH})


# Require OpenDaVINCI
set(OPENDAVINCI_DIR "/usr/local/include")
find_package(OpenDaVINCI REQUIRED)

# Require Automotivedata
set(AUTOMOTIVEDATA_DIR "/usr/local/include")
find_package(AutomotiveData REQUIRED)

set(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${OPENCV_LIBRARIES})

# Include dirs
include_directories(${OPENDAVINCI_INCLUDE_DIRS})
include_directories(${AUTOMOTIVEDATA_INCLUDE_DIRS})
include_directories(include)

# Build things?
file(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/miniature COMPONENT software)
