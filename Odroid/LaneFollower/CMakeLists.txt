cmake_minimum_required(VERSION 3.0)
project(LaneFollower)

# Set module paths, system dependant.
# TODO Make cmake search for it
set(CMAKE_MODULE_PATH "/usr/local/share/cmake-3.5/Modules" ${CMAKE_MODULE_PATH})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lopencv_core -lopencv_highgui -pedantic -Wall -Werror -Wextra -Wshadow -Wno-deprecated -Wfloat-equal -Wpointer-arith -Wwrite-strings -Wpacked")

SET (CXX_EFFECTIVE_CXX "-Wno-error=effc++ -Weffc++ -Wmissing-format-attribute
-Wredundant-decls ")

SET (CXX_WARNING_ALL "-Wcast-align -Wchar-subscripts -Wcomment -Wdisabledoptimization
-Wformat -Wformat=2 -Wformat-nonliteral -Wformat-security -Wformat-y2k -
Wfloat-equal -Winit-self -Winline -Winvalid-pch -Wlong-long -Wsign-compare -
Wuninitialized -Wunreachable-code -Wunsafe-loop-optimizations -Wunused -Wunused-function
-Wunused-label -Wunused-parameter -Wunused-but-set-parameter -Wunused-but-set-variable -
Wunused-value -Wunused-variable -Wno-maybe-uninitialized -Wunused-result -Wmissingbraces
-Wmissing-field-initializers -Wmissing-format-attribute -Wmissing-include-dirs -
Wmissing-noreturn -Wparentheses -Wsign-compare -Wswitch -Wuninitialized -Wunknownpragmas
-Wunreachable-code -Wtrigraphs -Wvariadic-macros -Wvolatile-register-var -
Wwrite-strings -Wpointer-arith -Wredundant-decls -Wreturn-type -Wsequence-point -Wstackprotector
-Wstrict-aliasing -Wstrict-aliasing=2 -Wsync-nand -Wsuggest-attribute=const -
Warray-bounds -Wtrampolines -Wlogical-op -Wnormalized=nfc -Wvarargs -Wvector-operationperformance
-Wvla -Wtype-limits -Wc++11-compat -Woverloaded-virtual")

# Require OpenDaVINCI
#set(OPENDAVINCI_DIR "/opt/od/")
find_package(OpenDaVINCI REQUIRED)

# Require Automotivedata
#set(AUTOMOTIVEDATA_DIR "/opt/od")
find_package(AutomotiveData REQUIRED)

# Find OpenCV.
set(OpenCV_ROOT_DIR "/usr/local")
find_package(OpenCV REQUIRED)

set(LIBRARIES ${OPENDAVINCI_LIBRARIES} ${AUTOMOTIVEDATA_LIBRARIES} ${OpenCV_LIBRARIES})

# Include dirs
include_directories(${OPENDAVINCI_INCLUDE_DIRS})
include_directories(${AUTOMOTIVEDATA_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(include)

# Build things?
file(GLOB_RECURSE thisproject-sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${PROJECT_NAME}lib-static STATIC ${thisproject-sources})
add_executable(${PROJECT_NAME} "${CMAKE_CURRENT_SOURCE_DIR}/apps/${PROJECT_NAME}.cpp")
target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}lib-static ${LIBRARIES})

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin/miniature COMPONENT software)
