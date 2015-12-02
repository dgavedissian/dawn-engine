# Locate RakNet
#
# The CMake variable RAKNET_DIR can be used to point this module to where the library
# is located.
#
# This module defines:
#  RAKNET_FOUND - Set to TRUE if the system has RakNet
#  RAKNET_INCLUDE_DIR - Set to the include directory
#  RAKNET_LIBRARIES - Set to the path of the library to link against

find_path(RAKNET_INCLUDE_DIR
    NAME RakNet/RakPeer.h
    PATHS
    ${RAKNET_DIR}/include
    ENV CPATH
    /usr/local/include
    /usr/include
    /sw/include
    /opt/local/include
    /opt/csw/include
    /opt/include
)

# Find the debug library
find_library(RAKNET_LIBRARY_DEBUG
    NAMES RakNetLibStaticDebug RakNetLibStatic_d
    PATHS
    ${RAKNET_DIR}/lib
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

# Find the optimized library
find_library(RAKNET_LIBRARY_RELEASE
    NAMES RakNetLibStatic
    PATHS
    ${RAKNET_DIR}/lib
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

find_package(OpenSSL)
set(RAKNET_INCLUDE_DIR ${RAKNET_INCLUDE_DIR} ${OPENSSL_INCLUDE_DIR})
if (WIN32)
    set(RAKNET_LIBRARIES
        optimized ${RAKNET_LIBRARY_RELEASE}
        debug ${RAKNET_LIBRARY_DEBUG})
    set(RAKNET_LIBRARIES ${RAKNET_LIBRARIES} ${OPENSSL_LIBRARIES} ws2_32.lib) # Raknet depends on Winsock 2
else()
    set(RAKNET_LIBRARIES ${OPENSSL_LIBRARIES} ${RAKNET_LIBRARY_RELEASE})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RakNet DEFAULT_MSG RAKNET_LIBRARIES RAKNET_INCLUDE_DIR)
mark_as_advanced(RAKNET_LIBRARY_RELEASE RAKNET_LIBRARY_DEBUG)
