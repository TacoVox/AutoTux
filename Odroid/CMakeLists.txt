CMAKE_MINIMUM_REQUIRED (VERSION 2.8)

PROJECT (AUTOTUX)

INCLUDE(ExternalProject)

######
SET(BUILD_PROXY 1)
SET(BUILD_LANEFOLLOWER 1)
SET(BUILD_DECISIONMAKER 1)
SET(BUILD_CONFIGURATOR 1)
######

# Build the Proxy
IF(${BUILD_PROXY})
    ExternalProject_Add (AutoTuxProxy
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/AutoTuxProxy/"
        INSTALL_COMMAND "")
ENDIF()

# Build the LaneFollower
IF(${BUILD_LANEFOLLOWER})
    ExternalProject_Add (LaneFollower
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/LaneFollower/"
        INSTALL_COMMAND "")
ENDIF()

# Build the DecisionMaker
IF(${BUILD_DECISIONMAKER})
    ExternalProject_Add (DecisionMaker
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/DecisionMaker/"
        INSTALL_COMMAND "")
ENDIF()

# Build ATConfigurator
IF(${BUILD_CONFIGURATOR})
    ExternalProject_Add (ATConfigurator
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ATConfigurator/"
        INSTALL_COMMAND "")
ENDIF()
