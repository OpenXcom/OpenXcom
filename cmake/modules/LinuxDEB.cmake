if ( "${CPACK_GENERATOR}" MATCHES "^(.*;)?DEB(;.*)?$" )
  set ( CPACK_DEBIAN_PACKAGE_MAINTAINER "The OpenXcom project" )
  set ( CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://openxcom.org/" )
  set ( CPACK_DEBIAN_PACKAGE_SECTION "games" )
  set ( CPACK_DEBIAN_PACKAGE_PRIORITY "optional" )

  if ( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64" )
    set ( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64" )
  elseif ( "${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "i686" )
    set ( CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386" )
  endif ()

  if ( NOT CPACK_DEBIAN_PACKAGE_DEPENDS )
    message ( STATUS "CPACK_DEBIAN_PACKAGE_DEPENDS not set - DEB package will have no dependencies" )
  endif ()

  set ( CPACK_DEBIAN_COMPRESSION_TYPE "xz" )
endif ()
