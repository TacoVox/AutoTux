cmake_minimum_required(VERSION 2.8)
project(LaneFollower)

# Set module paths, system dependant.
# TODO Make cmake search for it
set(CMAKE_MODULE_PATH "/opt/od/share/cmake-3.0/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lopencv_core -lopencv_highgui")

# SET warning flags here and append them to the CMAKE_CXX_FLAGS
SET(WARN "${WARN} -g -O -Wall -Weffc++ -pedantic -Wvolatile-register-var")
SET(WARN "${WARN} -Wconversion -Wcast-align -Wwrite-strings -Wunused-variable")
SET(WARN "${WARN} -Wcast-qual -Wchar-subscripts -Wvariadic-macros -Wimport")
SET(WARN "${WARN} -Wcomment -Wunused-parameter -Wextra -Wdisabled-optimization")
SET(WARN "${WARN} -Wfloat-equal -Wformat=2 -Wformat-nonliteral")
SET(WARN "${WARN} -Wformat-security -Wformat-y2k -Wwrite-strings")
SET(WARN "${WARN} -Winit-self -Wformat -Winline -Winvalid-pch -Wmissing-braces")
SET(WARN "${WARN} -Wunsafe-loop-optimizations -Wlong-long -Wreturn-type")
SET(WARN "${WARN} -Wmissing-field-initializers -Wmissing-format-attribute")
SET(WARN "${WARN} -Wmissing-include-dirs -Wpacked -Wparentheses -Wunused-value")
SET(WARN "${WARN} -Wmissing-noreturn -Wsequence-point -Wshadow -Wsign-compare")
SET(WARN "${WARN} -Wpointer-arith -Wredundant-decls -Wstack-protector -Wswitch")
SET(WARN "${WARN} -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch-enum -Wunused")
SET(WARN "${WARN} -Wunknown-pragmas -Wunreachable-code -Wswitch-default")
SET(WARN "${WARN} -Wunused-function -Wunused-label -Wtrigraphs -Wuninitialized")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN} -pedantic-errors -Werror")

# Require OpenDaVINCI
set(OPENDAVINCI_DIR "/opt/od")
find_package(OpenDaVINCI REQUIRED)

MESSAGE( STATUS "${OPENDAVINCI_DIR}" )

# Require Automotivedata
set(AUTOMOTIVEDATA_DIR "/opt/od")
find_package(AutomotiveData REQUIRED)

MESSAGE( STATUS "${AUTOMOTIVEDATA_DIR}" )

# Find OpenCV.
set(OpenCV_ROOT_DIR "/usr")
find_package(OpenCV REQUIRED)

set(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${OpenCV_LIBRARIES})

# Include dirs
include_directories(SYSTEM ${OPENDAVINCI_INCLUDE_DIRS})
include_directories(SYSTEM ${AUTOMOTIVEDATA_INCLUDE_DIRS})
include_directories(SYSTEM ${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Build things?
file(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/testsuite/*.h")
add_library(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/miniature COMPONENT software)
