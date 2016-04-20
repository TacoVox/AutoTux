CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (AUTOTUX)

INCLUDE(ExternalProject)

######
SET(BUILD_PROXY 1)
######

# Build the Proxy
IF(${BUILD_PROXY})
    ExternalProject_Add (AutoTuxProxy
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AutoTuxProxy/"
        INSTALL_COMMAND ""
)
ENDIF()
