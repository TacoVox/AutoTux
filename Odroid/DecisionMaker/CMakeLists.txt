# Set the CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project name
PROJECT(DecisionMaker)

SET (CMAKE_MODULE_PATH "/opt/od/share/cmake-2.8/Modules" ${CMAKE_MODULE_PATH})
SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11")
#[[SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O -Wall -Weffc++ -pedantic
    -pedantic-errors -Wextra -Waggregate-return -Wcast-align
    -Wcast-qual  -Wchar-subscripts  -Wcomment -Wconversion
    -Wdisabled-optimization
    -Werror -Wfloat-equal  -Wformat  -Wformat=2
    -Wformat-nonliteral -Wformat-security
    -Wformat-y2k
    -Wimplicit  -Wimport  -Winit-self  -Winline
    -Winvalid-pch
    -Wunsafe-loop-optimizations  -Wlong-long -Wmissing-braces
    -Wmissing-field-initializers -Wmissing-format-attribute
    -Wmissing-include-dirs -Wmissing-noreturn
    -Wpacked  -Wpadded -Wparentheses  -Wpointer-arith
    -Wredundant-decls -Wreturn-type
    -Wsequence-point  -Wshadow -Wsign-compare  -Wstack-protector
    -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch  -Wswitch-default
    -Wswitch-enum -Wtrigraphs  -Wuninitialized
    -Wunknown-pragmas  -Wunreachable-code -Wunused
    -Wunused-function  -Wunused-label  -Wunused-parameter
    -Wunused-value  -Wunused-variable  -Wvariadic-macros
    -Wvolatile-register-var  -Wwrite-strings")]]

# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

# Find Automotivedata
SET(AUTOMOTIVEDATA_DIR "/opt/od")
FIND_PACKAGE(AutomotiveData REQUIRED)

# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES(${OPENDAVINCI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${AUTOMOTIVEDATA_INCLUDE_DIRS})
# Set include directory.
INCLUDE_DIRECTORIES(include)

SET(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES})

FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h")
ADD_LIBRARY(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
ADD_EXECUTABLE(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})
