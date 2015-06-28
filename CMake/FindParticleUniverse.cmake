# Locate the Particle Universe plugin for Ogre
#
# The CMake variable PARTICLEUNIVERSE_DIR can be used to point this module to where the library
# is located.
#
# This module defines:
#  PARTICLEUNIVERSE_FOUND - Set to TRUE if the system has Particle Universe
#  PARTICLEUNIVERSE_INCLUDE_DIR - Set to the include directory
#  PARTICLEUNIVERSE_LIBRARIES - Set to the path of the library to link against

find_path(PARTICLEUNIVERSE_INCLUDE_DIR
  ParticleUniversePlugin.h
  PATHS
  ${PARTICLEUNIVERSE_DIR}/include
  ENV CPATH
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include
  /opt/include
)

# The lib prefix isn't used here
set(CMAKE_FIND_LIBRARY_PREFIXES ${CMAKE_FIND_LIBRARY_PREFIXES} "")

# Find the debug library
find_library(PARTICLEUNIVERSE_LIBRARY_DEBUG
  NAMES Plugin_ParticleUniverse_d
  PATHS
  ENV LD_LIBRARY_PATH
  ENV LIBRARY_PATH
  ${PARTICLEUNIVERSE_DIR}/lib
  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

# Find the optimized library
find_library(PARTICLEUNIVERSE_LIBRARY_RELEASE
  NAMES Plugin_ParticleUniverse
  PATHS
  ENV LD_LIBRARY_PATH
  ENV LIBRARY_PATH
  ${PARTICLEUNIVERSE_DIR}/lib
  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

if (WIN32)
  set(PARTICLEUNIVERSE_LIBRARIES
    optimized ${PARTICLEUNIVERSE_LIBRARY_RELEASE}
    debug ${PARTICLEUNIVERSE_LIBRARY_DEBUG})
else()
    set(PARTICLEUNIVERSE_LIBRARIES ${PARTICLEUNIVERSE_LIBRARY_RELEASE})
endif()
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ParticleUniverse DEFAULT_MSG PARTICLEUNIVERSE_LIBRARIES PARTICLEUNIVERSE_INCLUDE_DIR)
mark_as_advanced(PARTICLEUNIVERSE_LIBRARY_RELEASE PARTICLEUNIVERSE_LIBRARY_DEBUG)