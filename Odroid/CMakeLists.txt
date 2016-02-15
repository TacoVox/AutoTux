# autotux
# Include license.

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (AutoTux)

###########################################################################
# Search path for .cmake files
SET (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake.Modules" ${CMAKE_MODULE_PATH})

# Add a local CMake module search path dependent on the desired installation
# destination.
# Thus, artifacts from the complete source build can be given precendence over
# any installed versions.
IF(UNIX)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/share/cmake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()
IF(WIN32)
    SET (CMAKE_MODULE_PATH "${CMAKE_INSTALL_PREFIX}/CMake-${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}/Modules" ${CMAKE_MODULE_PATH})
ENDIF()

###########################################################################
# Include compile flags
INCLUDE (CompileFlags)

###########################################################################
# Find OpenDaVINCI.
SET(OPENDAVINCI_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (OpenDaVINCI REQUIRED)

###########################################################################
# Find AutomotiveData.
SET(AUTOMOTIVEDATA_DIR "${CMAKE_INSTALL_PREFIX}")
FIND_PACKAGE (AutomotiveData REQUIRED)

###########################################################################
# Set header files from OpenDaVINCI.
INCLUDE_DIRECTORIES (${OPENDAVINCI_INCLUDE_DIRS}/opendavinci)
# Set header files from AutomotiveData.
INCLUDE_DIRECTORIES (${AUTOMOTIVEDATA_INCLUDE_DIRS}/automotivedata)
# Set include directory.
INCLUDE_DIRECTORIES(include)

#################################################################
# Build project
ADD_EXCECUTABLE ($(PROJECT_NAME) "")
