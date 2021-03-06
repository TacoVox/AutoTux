# Set the CMake version
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

# Project name
PROJECT(ATConfigurator)

SET(CMAKE_MODULE_PATH "/opt/od/share/cmake-2.8/Modules" ${CMAKE_MODULE_PATH})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${CXX_OPTIONS} -pipe -std=c++11 -lncurses -lmenu -lopencv_core")
# SET warning flags here and append them to the CMAKE_CXX_FLAGS
SET(WARN "${WARN} -g -O -Wall -pedantic -Wvolatile-register-var")
SET(WARN "${WARN} -Wconversion -Wcast-align -Wwrite-strings -Wunused-variable")
SET(WARN "${WARN} -Wcast-qual -Wchar-subscripts -Wvariadic-macros -Wimport")
SET(WARN "${WARN} -Wcomment -Wunused-parameter -Wextra -Wdisabled-optimization")
SET(WARN "${WARN} -Wfloat-equal -Wformat=2 -Wformat-nonliteral")
SET(WARN "${WARN} -Wformat-security -Wformat-y2k -Wwrite-strings")
SET(WARN "${WARN} -Winit-self -Wformat -Winline -Winvalid-pch -Wmissing-braces")
SET(WARN "${WARN} -Wlong-long -Wreturn-type")
SET(WARN "${WARN} -Wmissing-field-initializers -Wmissing-format-attribute")
SET(WARN "${WARN} -Wmissing-include-dirs -Wpacked -Wparentheses -Wunused-value")
SET(WARN "${WARN} -Wmissing-noreturn -Wsequence-point -Wshadow -Wsign-compare")
SET(WARN "${WARN} -Wpointer-arith -Wredundant-decls -Wstack-protector -Wswitch")
SET(WARN "${WARN} -Wstrict-aliasing -Wstrict-aliasing=2 -Wswitch-enum -Wunused")
SET(WARN "${WARN} -Wunknown-pragmas -Wunreachable-code -Wswitch-default")
SET(WARN "${WARN} -Wunused-function -Wunused-label -Wtrigraphs -Wuninitialized")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN} -pedantic-errors -Werror")

# Find OpenDaVINCI
SET(OPENDAVINCI_DIR "/opt/od")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

# Find Automotivedata
SET(AUTOMOTIVEDATA_DIR "/opt/od")
FIND_PACKAGE(AutomotiveData REQUIRED)

#Find ncurses
FIND_PACKAGE(Curses REQUIRED)
SET(CURSES_USE_NCURSES TRUE)

# Find OpenCV
SET(OpenCV_ROOT_DIR "/usr")
FIND_PACKAGE(OpenCV REQUIRED)

# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES(SYSTEM ${OPENDAVINCI_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${AUTOMOTIVEDATA_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(SYSTEM ${CURSES_INCLUDE_DIR})
INCLUDE_DIRECTORIES(SYSTEM ${OpenCV_INCLUDE_DIR})
# Set include directory.
INCLUDE_DIRECTORIES(include)

SET(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${CURSES_LIBRARIES} ${OpenCV_LIBRARIES})

FILE(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/include/*/*.h")
ADD_LIBRARY(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
ADD_EXECUTABLE(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})