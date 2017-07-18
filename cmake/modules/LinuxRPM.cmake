if ( "${CPACK_GENERATOR}" MATCHES "^(.*;)?RPM(;.*)?$" )
  set ( CPACK_RPM_PACKAGE_LICENSE "GPLv3" )
  set ( CPACK_RPM_PACKAGE_GROUP "Amusements/Games" )
  set ( CPACK_RPM_PACKAGE_URL "https://openxcom.org/" )

  if ( NOT CPACK_RPM_PACKAGE_REQUIRES )
    message ( STATUS "CPACK_RPM_PACKAGE_REQUIRES not set - RPM package will have no dependencies" )
  endif ()

  set ( CPACK_RPM_COMPRESSION_TYPE "xz" )
endif ()
