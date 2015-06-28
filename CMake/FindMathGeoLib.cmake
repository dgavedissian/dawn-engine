# - Locate MathGeoLib library
# This module defines
#  MATHGEOLIB_LIBRARY, the library to link against
#  MATHGEOLIB_FOUND, if false, do not try to link to LUABIND
#  MATHGEOLIB_INCLUDE_DIR, where to find headers.
# Use the define MATHGEOLIB_DIR to tell this module where else to look

IF(MATHGEOLIB_LIBRARY AND MATHGEOLIB_INCLUDE_DIR)
  # in cache already
  SET(MATHGEOLIB_FIND_QUIETLY TRUE)
ENDIF(MATHGEOLIB_LIBRARY AND MATHGEOLIB_INCLUDE_DIR)

FIND_PATH(MATHGEOLIB_INCLUDE_DIR
  MathGeoLib.h
  PATHS
  ${MATHGEOLIB_DIR}/include
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include
  /opt/include
  PATH_SUFFIXES MathGeoLib
)

FIND_LIBRARY(MATHGEOLIB_LIBRARY_DEBUG
  NAMES MathGeoLib_d
  PATHS
  ${MATHGEOLIB_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

FIND_LIBRARY(MATHGEOLIB_LIBRARY_RELEASE
  NAMES MathGeoLib
  PATHS
  ${MATHGEOLIB_DIR}/lib
  /usr/local/lib
  /usr/lib
  /usr/local/X11R6/lib
  /usr/X11R6/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

IF ((CMAKE_CONFIGURATION_TYPES OR CMAKE_BUILD_TYPE) AND WIN32)
  SET(MATHGEOLIB_LIBRARY optimized ${MATHGEOLIB_LIBRARY_RELEASE} debug ${MATHGEOLIB_LIBRARY_DEBUG})
ELSE()
  # if there are no configuration types and CMAKE_BUILD_TYPE has no value
  # then just use the release libraries
  SET(MATHGEOLIB_LIBRARY ${MATHGEOLIB_LIBRARY_RELEASE} )
ENDIF()

IF(MATHGEOLIB_LIBRARY AND MATHGEOLIB_INCLUDE_DIR)
  SET(MATHGEOLIB_FOUND "YES")
  IF(NOT MATHGEOLIB_FIND_QUIETLY)
    MESSAGE(STATUS "Found MathGeoLib: ${MATHGEOLIB_LIBRARY}")
  ENDIF(NOT MATHGEOLIB_FIND_QUIETLY)
ELSE(MATHGEOLIB_LIBRARY AND MATHGEOLIB_INCLUDE_DIR)
  IF(NOT MATHGEOLIB_FIND_QUIETLY)
    MESSAGE(FATAL_ERROR "Unable to find MathGeoLib!")
  ENDIF(NOT MATHGEOLIB_FIND_QUIETLY)
ENDIF(MATHGEOLIB_LIBRARY AND MATHGEOLIB_INCLUDE_DIR)
