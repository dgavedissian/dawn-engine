# Locate the IrrKlang audio library
#
# The CMake variable IRRKLANG_DIR can be used to point this module to where the library
# is located.
#
# This module defines:
#  IRRKLANG_FOUND - Set to TRUE if the system has particle universe
#  IRRKLANG_INCLUDE_DIR - Set to the include directory
#  IRRKLANG_LIBRARIES - Set to the path of the library to link against

find_path(IRRKLANG_INCLUDE_DIR
  irrKlang.h
  PATHS
  ${IRRKLANG_DIR}/include
  ENV CPATH
  /usr/local/include
  /usr/include
  /sw/include
  /opt/local/include
  /opt/csw/include
  /opt/include
)

# Find the optimized library
find_library(IRRKLANG_LIBRARY_RELEASE
  NAMES irrKlang IrrKlang
  PATHS
  ${IRRKLANG_DIR}/lib
  ENV LD_LIBRARY_PATH
  ENV LIBRARY_PATH
  /usr/local/lib
  /usr/lib
  /sw/lib
  /opt/local/lib
  /opt/csw/lib
  /opt/lib
  /usr/freeware/lib64
)

set(IRRKLANG_LIBRARIES ${IRRKLANG_LIBRARY_RELEASE})
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(IrrKlang DEFAULT_MSG IRRKLANG_LIBRARIES IRRKLANG_INCLUDE_DIR)
mark_as_advanced(IRRKLANG_LIBRARY_RELEASE)
